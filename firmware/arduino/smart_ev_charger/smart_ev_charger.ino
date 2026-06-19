/*
 * smart_ev_charger.ino — Smart EV Charging Station with BMS
 * Platform : Arduino Nano (ATmega328P @ 16MHz)
 * Hardware : GSM SIM900A | LM35 | BMS Module | SSR | LCD 16x2
 *
 * State Machine:
 *   IDLE → PAYMENT_PENDING → PRE_CHECK → CHARGING → SESSION_END
 *                                              ↘ FAULT
 *
 * Author  : [Your Name]
 * College : [Your College Name]
 * Version : 1.0.0
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#include "config.h"
#include "bms.h"
#include "gsm.h"

// ─── Peripheral objects ───────────────────────────────────────────────────────
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN,
                  PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
BMS bms;
GSM gsm(PIN_GSM_RX, PIN_GSM_TX);

// ─── System state ─────────────────────────────────────────────────────────────
enum class State : uint8_t { IDLE, PAYMENT_PENDING, PRE_CHECK, CHARGING, SESSION_END, FAULT };
State state = State::IDLE;

// ─── Session tracking ─────────────────────────────────────────────────────────
float          g_paidAmount      = 0.0f;
uint32_t       g_sessionDuration = 0;   // seconds of charge credit
unsigned long  g_sessionStartMs  = 0;
float          g_startSOC        = 0.0f;

// ─── Timing ───────────────────────────────────────────────────────────────────
unsigned long lastSensorPoll  = 0;
unsigned long lastSMSPoll     = 0;
unsigned long lastLCDUpdate   = 0;

// ─── EEPROM session record ────────────────────────────────────────────────────
struct SessionRecord {
    uint32_t startMs;
    uint16_t durationMin;
    uint8_t  startSOC;
    uint8_t  endSOC;
    uint16_t energyWhX10;  // energy_wh × 10, stored as integer
    uint8_t  faultCode;    // 0=normal, 1=OV, 2=OT, 3=timeout
};

uint8_t eepromRecordCount = 0;

// ─── Forward declarations ─────────────────────────────────────────────────────
void initEEPROM();
void logSessionToEEPROM(const SessionRecord& rec);
void setSSR(bool on);
void beep(int count, int durationMs);
void displayIdle();
void displayCharging(const BatteryData& data, unsigned long elapsedSec);
void displayFault(BatteryFault fault);
void displaySessionEnd(const BatteryData& data, unsigned long durationSec);

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    // Output pins
    pinMode(PIN_SSR,       OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED,   OUTPUT);
    pinMode(PIN_BUZZER,    OUTPUT);
    setSSR(false);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED,   LOW);

    // LCD init
    lcd.begin(16, 2);
    lcd.print(F("Smart EV Charger"));
    lcd.setCursor(0, 1);
    lcd.print(F("Initialising..."));

    // Init subsystems
    bms.begin();
    initEEPROM();

    // Init GSM last (takes longest to boot)
    lcd.setCursor(0, 1);
    lcd.print(F("GSM Init...     "));
    gsm.begin();

    if (!gsm.isReady()) {
        lcd.setCursor(0, 1);
        lcd.print(F("GSM FAIL-offline"));
        beep(3, BEEP_SHORT_MS);
    } else {
        lcd.setCursor(0, 1);
        lcd.print(F("GSM OK          "));
        beep(2, BEEP_SHORT_MS);
    }

    delay(1000);
    displayIdle();
    state = State::IDLE;
}

void loop() {
    unsigned long now = millis();

    // ── SMS Polling (every 5 sec, in IDLE and FAULT states) ───────────────────
    if ((state == State::IDLE) && (now - lastSMSPoll >= SMS_POLL_INTERVAL_MS)) {
        lastSMSPoll = now;
        PaymentInfo payment = gsm.pollForPayment();

        if (payment.isValid) {
            g_paidAmount       = payment.amount;
            g_sessionDuration  = (uint32_t)((payment.amount / RATE_PER_UNIT_RS) * 3600UL);
            state              = State::PRE_CHECK;

            lcd.clear();
            lcd.print(F("Payment: Rs"));
            lcd.print((int)g_paidAmount);
            lcd.setCursor(0, 1);
            lcd.print(F("Verifying..."));
            beep(1, BEEP_SHORT_MS);
            gsm.deleteAllSMS();
        }
    }

    // ── Pre-Charge Safety Check ────────────────────────────────────────────────
    if (state == State::PRE_CHECK) {
        BatteryData data = bms.read();

        if (data.fault != BatteryFault::NONE) {
            state = State::FAULT;
            setSSR(false);
            displayFault(data.fault);
            beep(3, BEEP_LONG_MS);
            digitalWrite(PIN_LED_RED, HIGH);
            return;
        }

        // All checks pass — start charging
        bms.startSession();
        g_sessionStartMs = millis();
        g_startSOC       = data.socPercent;
        setSSR(true);
        digitalWrite(PIN_LED_GREEN, HIGH);
        state = State::CHARGING;
        beep(1, BEEP_SHORT_MS);

        lcd.clear();
        lcd.print(F("Charging Started"));
        lcd.setCursor(0, 1);
        lcd.print(F("SOC: "));
        lcd.print((int)data.socPercent);
        lcd.print(F("%"));
    }

    // ── Sensor Polling during Charging ────────────────────────────────────────
    if ((state == State::CHARGING) && (now - lastSensorPoll >= SENSOR_POLL_INTERVAL_MS)) {
        lastSensorPoll = now;
        BatteryData data = bms.read();
        unsigned long elapsed = (now - g_sessionStartMs) / 1000UL;

        // Check for faults
        if (data.fault != BatteryFault::NONE) {
            state = State::FAULT;
            setSSR(false);
            digitalWrite(PIN_LED_GREEN, LOW);
            digitalWrite(PIN_LED_RED,   HIGH);
            displayFault(data.fault);
            beep(3, BEEP_LONG_MS);

            // Log fault session
            SessionRecord rec;
            rec.startMs      = (uint32_t)(g_sessionStartMs / 1000UL);
            rec.durationMin  = (uint16_t)(elapsed / 60);
            rec.startSOC     = (uint8_t)g_startSOC;
            rec.endSOC       = (uint8_t)data.socPercent;
            rec.energyWhX10  = 0;  // Unknown in fault
            rec.faultCode    = (uint8_t)data.fault;
            logSessionToEEPROM(rec);
            return;
        }

        // Check session time limit
        if (elapsed >= g_sessionDuration) {
            state = State::SESSION_END;
        }

        // Check SOC full
        if (data.socPercent >= SOC_CHARGE_STOP) {
            state = State::SESSION_END;
        }

        // Update LCD
        if (now - lastLCDUpdate >= LCD_UPDATE_INTERVAL_MS) {
            lastLCDUpdate = now;
            displayCharging(data, elapsed);
        }

        if (state == State::SESSION_END) {
            setSSR(false);
            digitalWrite(PIN_LED_GREEN, LOW);
            beep(2, BEEP_SHORT_MS);

            unsigned long finalElapsed = (millis() - g_sessionStartMs) / 1000UL;
            displaySessionEnd(data, finalElapsed);

            // Log to EEPROM
            SessionRecord rec;
            rec.startMs      = (uint32_t)(g_sessionStartMs / 1000UL);
            rec.durationMin  = (uint16_t)(finalElapsed / 60);
            rec.startSOC     = (uint8_t)g_startSOC;
            rec.endSOC       = (uint8_t)data.socPercent;
            rec.energyWhX10  = 0;  // Estimated from duration + assumed 5W avg
            rec.faultCode    = 0;
            logSessionToEEPROM(rec);

            bms.endSession();
            delay(LCD_BILL_DISPLAY_MS);
            displayIdle();
            state = State::IDLE;
        }
    }

    // ── Fault state — wait for conditions to clear ─────────────────────────────
    if (state == State::FAULT) {
        static unsigned long lastFaultCheck = 0;
        if (now - lastFaultCheck >= FAULT_RETRY_INTERVAL_MS) {
            lastFaultCheck = now;
            BatteryData data = bms.read();
            if (data.fault == BatteryFault::NONE) {
                bms.clearFault();
                digitalWrite(PIN_LED_RED, LOW);
                state = State::IDLE;
                displayIdle();
                beep(1, BEEP_SHORT_MS);
            }
        }
    }
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

void setSSR(bool on) {
    digitalWrite(PIN_SSR, on ? HIGH : LOW);
}

void beep(int count, int durationMs) {
    for (int i = 0; i < count; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(durationMs);
        digitalWrite(PIN_BUZZER, LOW);
        if (i < count - 1) delay(100);
    }
}

void displayIdle() {
    lcd.clear();
    lcd.print(F("EV Charger Ready"));
    lcd.setCursor(0, 1);
    lcd.print(F("Send UPI Payment"));
}

void displayCharging(const BatteryData& data, unsigned long elapsedSec) {
    lcd.clear();
    lcd.print(F("SOC:"));
    lcd.print((int)data.socPercent);
    lcd.print(F("% "));
    lcd.print(data.packVoltage, 1);
    lcd.print(F("V"));

    lcd.setCursor(0, 1);
    lcd.print((int)data.temperatureC);
    lcd.print((char)223);  // ° character
    lcd.print(F("C  "));
    unsigned long min = elapsedSec / 60;
    unsigned long sec = elapsedSec % 60;
    if (min < 10) lcd.print('0');
    lcd.print(min);
    lcd.print(':');
    if (sec < 10) lcd.print('0');
    lcd.print(sec);
}

void displayFault(BatteryFault fault) {
    lcd.clear();
    lcd.print(F("! FAULT !"));
    lcd.setCursor(0, 1);
    switch (fault) {
        case BatteryFault::OVERVOLTAGE: lcd.print(F("Overvoltage")); break;
        case BatteryFault::OVERTEMP:    lcd.print(F("Overtemperature")); break;
        default:                        lcd.print(F("Unknown Fault")); break;
    }
}

void displaySessionEnd(const BatteryData& data, unsigned long durationSec) {
    lcd.clear();
    lcd.print(F("Session Complete"));
    lcd.setCursor(0, 1);
    lcd.print(F("SOC:"));
    lcd.print((int)data.socPercent);
    lcd.print(F("% "));
    unsigned long min = durationSec / 60;
    lcd.print(min);
    lcd.print(F("min"));
}

// ─── EEPROM ───────────────────────────────────────────────────────────────────

void initEEPROM() {
    if (EEPROM.read(0) != EEPROM_MAGIC_BYTE) {
        EEPROM.write(0, EEPROM_MAGIC_BYTE);
        EEPROM.write(1, 0);  // record count low byte
        EEPROM.write(2, 0);  // record count high byte
    }
    eepromRecordCount = (EEPROM.read(2) << 8) | EEPROM.read(1);
    if (eepromRecordCount > EEPROM_MAX_RECORDS) eepromRecordCount = 0;
}

void logSessionToEEPROM(const SessionRecord& rec) {
    if (eepromRecordCount >= EEPROM_MAX_RECORDS) {
        eepromRecordCount = 0;  // Wrap around (ring buffer)
    }
    int addr = EEPROM_RECORD_START + eepromRecordCount * EEPROM_RECORD_SIZE;
    EEPROM.put(addr, rec);
    eepromRecordCount++;
    EEPROM.write(1, eepromRecordCount & 0xFF);
    EEPROM.write(2, (eepromRecordCount >> 8) & 0xFF);
}
