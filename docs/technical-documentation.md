# Technical Documentation — Smart EV Charging Station with BMS

## Firmware Module Reference

### `config.h`

Centralizes all hardware pin assignments and tunable constants. Every threshold is defined here so the system can be recalibrated without touching logic code.

**Key sections:**

| Section | Constants | Description |
|---|---|---|
| Pin Assignments | `PIN_GSM_RX/TX`, `PIN_SSR`, `PIN_LCD_*`, `PIN_TEMP_ADC`, `PIN_VOLT_ADC` | All hardware connections |
| Battery Config | `PACK_VOLT_FULL`, `PACK_VOLT_MIN`, `CELLS_IN_SERIES` | 3S Li-ion pack parameters |
| Safety Thresholds | `TEMP_CUTOFF_C`, `VOLT_OVERVOLT_SW` | Emergency cutoff values |
| Voltage Divider | `VDIV_SCALE_FACTOR`, `ADC_VREF` | ADC → voltage calculation |
| Payment Config | `RATE_PER_UNIT_RS`, `MIN_PAYMENT_RS` | Payment validation rules |
| GSM Config | `GSM_BAUD_RATE`, `SMS_POLL_INTERVAL_MS` | Serial timing |
| Timing | `SENSOR_POLL_INTERVAL_MS`, `LCD_UPDATE_INTERVAL_MS` | Non-blocking timer intervals |
| EEPROM Layout | `EEPROM_RECORD_START`, `EEPROM_RECORD_SIZE` | Persistent storage offsets |

---

### `bms.h` / `bms.cpp`

Handles all sensor reading and battery safety evaluation.

**Public API:**

```cpp
class BMS {
    void begin();             // Initialize ADC pins
    BatteryData read();       // Read voltage + temp, estimate SOC, check faults
    void startSession();      // Reset fault state at session start
    void endSession();        // Session cleanup
    bool isFaultActive();     // Query fault state
    BatteryFault getFaultType(); // Get specific fault type
    void clearFault();        // Reset after fault condition clears
};
```

**`BatteryData` struct fields:**

| Field | Type | Description |
|---|---|---|
| `packVoltage` | `float` | Actual 3S pack voltage in Volts |
| `temperatureC` | `float` | Filtered cell temperature from LM35 in °C |
| `socPercent` | `float` | Estimated SOC 0–100% |
| `fault` | `BatteryFault` | `NONE`, `OVERVOLTAGE`, or `OVERTEMP` |

**`readPackVoltage()` implementation:**

```cpp
// ADC pin A1 → voltage divider output
// Pack voltage = (adc_raw × Vref / resolution) / scale_factor
float BMS::readPackVoltage() {
    int raw = analogRead(PIN_VOLT_ADC);
    float adcV = raw * (5.0f / 1024.0f);
    return adcV / VDIV_SCALE_FACTOR;  // = adcV / 0.3197
}
```

**`readTemperature()` implementation:**

```cpp
// ADC pin A0 → LM35 (10mV/°C)
// Running average over 4 samples filters ADC noise
float BMS::readTemperature() {
    int raw = analogRead(PIN_TEMP_ADC);
    float tempC = (raw * (5.0f / 1024.0f)) * 100.0f;
    _tempBuf[_tempIdx++ % 4] = tempC;
    float sum = 0;
    for (int i = 0; i < 4; i++) sum += _tempBuf[i];
    return sum / 4.0f;
}
```

**`estimateSOC()` — Voltage-SOC lookup table:**

The lookup table maps 3S pack voltage to estimated SOC based on the Li-ion discharge curve at approximately 0.2C. This is an approximation — accuracy is ±5–10% due to the non-linear curve and temperature effects.

```
12.60V → 100%    12.00V → 75%    11.40V → 45%    10.80V → 15%
12.45V →  95%    11.85V → 67.5%  11.25V → 37.5%  10.50V → 10%
12.30V →  90%    11.70V → 60%    11.10V → 30%     9.00V →  5%
12.15V →  82.5%  11.55V → 52.5%  10.95V → 22.5%
```

---

### `gsm.h` / `gsm.cpp`

Encapsulates all GSM SIM900A communication — initialization, SMS polling, payment parsing, and outgoing SMS.

**Public API:**

```cpp
class GSM {
    void begin();              // Initialize SoftwareSerial, send AT init sequence
    bool isReady();            // Returns true if module responded to AT
    PaymentInfo pollForPayment(); // Poll for payment SMS, parse, return result
    bool sendSMS(const char* number, const char* msg); // Send confirmation SMS
    void deleteAllSMS();       // Clean up after processing
    const char* getLastError(); // Human-readable error string
};
```

**`PaymentInfo` struct:**

```cpp
struct PaymentInfo {
    bool  isValid;          // True if valid payment found
    float amount;           // Parsed rupee amount
    char  senderHint[16];   // Last 4 digits of sender for logging
};
```

**Payment parsing logic:**

1. Send `AT+CMGL="ALL"` to SIM900A
2. Wait for response (up to 3 seconds)
3. Check for `+CMGL:` header — if absent, no messages
4. Extract SMS body text after the header line
5. Scan for payment-positive keywords: `credited`, `received`, `paid`, `Payment of`, `Rs.`, `INR`
6. Reject if debit/failure keywords found: `debited`, `failed`, `declined`, `reversed`
7. Extract numerical amount using `extractAmount()` — finds `Rs.XX` or `INR XX` pattern
8. Validate: `MIN_PAYMENT_RS ≤ amount ≤ MAX_PAYMENT_RS`
9. Return valid `PaymentInfo` if all checks pass

---

### `smart_ev_charger.ino` — Main State Machine

The main loop implements a non-blocking state machine using `millis()` for timing. No `delay()` is used in the main loop (only in one-time initialization steps).

**State transitions:**

```
IDLE ──(valid payment SMS)──→ PRE_CHECK
PRE_CHECK ──(safety pass)───→ CHARGING
PRE_CHECK ──(safety fail)───→ FAULT
CHARGING ──(SOC ≥ 98%)──────→ SESSION_END
CHARGING ──(time elapsed)───→ SESSION_END
CHARGING ──(fault detected)─→ FAULT
SESSION_END ─────────────────→ IDLE  (after LCD display)
FAULT ──(condition clears)──→ IDLE   (auto-retry every 30s)
```

**Non-blocking timing pattern used throughout:**

```cpp
// Pattern: check elapsed time before executing periodic action
if (millis() - lastSensorPoll >= SENSOR_POLL_INTERVAL_MS) {
    lastSensorPoll = millis();
    // ... do sensor work
}
```

This avoids `delay()` blocking which would miss RFID/SMS events during a charging session.

---

## Flash Usage Estimate

| Module | Approx. Flash | Notes |
|---|---|---|
| Arduino core + LiquidCrystal | ~8 KB | Base framework |
| SoftwareSerial | ~2 KB | GSM UART |
| BMS logic + SOC table | ~2 KB | Floating point math |
| GSM driver + string parsing | ~4 KB | AT commands + SMS parsing |
| Main state machine | ~3 KB | Control flow |
| EEPROM + LCD display | ~2 KB | Persistent storage + display |
| **Total estimated** | **~21 KB** | Out of 32KB available |

---

## EEPROM Data Format

EEPROM layout (Arduino Nano, 1024 bytes total):

```
Byte 0     : Magic byte (0xEA) — validates EEPROM is initialized
Byte 1–2   : Record count (little-endian uint16_t)
Byte 3     : Reserved
Byte 4–14  : SessionRecord[0] — 11 bytes
Byte 15–25 : SessionRecord[1]
...
Byte 994–1004 : SessionRecord[90]  (max 90 records)
```

`SessionRecord` layout (11 bytes):

```
Bytes 0–3  : startMs (uint32_t) — session start in millis/1000
Bytes 4–5  : durationMin (uint16_t) — session duration in minutes
Byte  6    : startSOC (uint8_t) — SOC at session start (%)
Byte  7    : endSOC (uint8_t) — SOC at session end (%)
Bytes 8–9  : energyWhX10 (uint16_t) — energy × 10 (e.g. 52 = 5.2 Wh)
Byte  10   : faultCode (uint8_t) — 0=normal, 1=OV, 2=OT, 3=timeout
```

Records use a ring-buffer strategy: when 90 records are filled, the counter resets to 0 and overwrites from the beginning.

---

## Known Limitations

| Limitation | Root Cause | Workaround / Future Fix |
|---|---|---|
| SOC accuracy ±5–10% | Voltage-only estimation; no current sensing | V2: Add ACS712 or INA219 for coulomb counting |
| SoftwareSerial @ 9600 baud | ATmega328P can't handle higher rates reliably with SW serial | V2: ESP32 hardware UART |
| Single SMS per poll cycle | Buffer limited to one SMS at a time | Delete processed SMS immediately; retry polls |
| No per-user authentication | No RFID or login in V1 | V2: RFID authentication |
| 2G only | SIM900A hardware limitation | V2: Replace with SIM7600 (4G) or use ESP32 Wi-Fi |
| Temperature sensor is ambient-adjacent | LM35 not directly on cell surface | Mount LM35 directly on battery pack in V2 |
| Energy measurement is estimated | No current sensor | Improvement: ACS712 for actual power × time |
