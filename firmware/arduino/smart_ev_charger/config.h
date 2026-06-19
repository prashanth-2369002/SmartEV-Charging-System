/*
 * config.h — Smart EV Charging Station with BMS
 * Platform: Arduino Nano (ATmega328P)
 * Hardware: GSM SIM900A + LM35 + BMS + SSR + LCD 16x2
 */

#ifndef CONFIG_H
#define CONFIG_H

// ─── Pin Assignments ──────────────────────────────────────────────────────────
// D12/D13 used for SoftwareSerial so D0/D1 (hardware UART) stays free for
// Serial.print() debug output over USB — do NOT move these to D0/D1.
#define PIN_GSM_RX        12   // SoftwareSerial RX (← SIM900A TX)
#define PIN_GSM_TX        13   // SoftwareSerial TX (→ SIM900A RX)

#define PIN_LCD_RS        2
#define PIN_LCD_EN        3
#define PIN_LCD_D4        4
#define PIN_LCD_D5        5
#define PIN_LCD_D6        6
#define PIN_LCD_D7        7

#define PIN_SSR           8    // HIGH = relay ON = charging active
#define PIN_LED_GREEN     9    // Session active indicator
#define PIN_LED_RED       10   // Fault / alert indicator
#define PIN_BUZZER        11   // Active buzzer

#define PIN_TEMP_ADC      A0   // LM35 output → Arduino A0
#define PIN_VOLT_ADC      A1   // Voltage divider output → Arduino A1

// ─── Battery Configuration (3S Li-ion 18650) ─────────────────────────────────
#define CELLS_IN_SERIES       3
#define CELL_VOLT_FULL        4.20f   // Volts per cell at full charge
#define CELL_VOLT_MIN         2.75f   // Volts per cell at deep discharge limit
#define PACK_VOLT_FULL        (CELLS_IN_SERIES * CELL_VOLT_FULL)   // 12.60V
#define PACK_VOLT_MIN         (CELLS_IN_SERIES * CELL_VOLT_MIN)    // 8.25V
#define SOC_CHARGE_STOP       98.0f   // % — stop charging at 98% (avoid full stress)

// ─── Safety Thresholds ────────────────────────────────────────────────────────
#define TEMP_CUTOFF_C         45.0f   // °C — emergency overtemp cutoff
#define TEMP_WARNING_C        40.0f   // °C — warning (show on LCD, no cutoff)
#define VOLT_OVERVOLT_SW      12.65f  // V — software overvoltage fault threshold

// ─── Voltage Divider Calibration ─────────────────────────────────────────────
// R1 = 10kΩ (upper leg), R2 = 4.7kΩ (lower leg)
// Scale factor = R2 / (R1 + R2) = 4700 / 14700 = 0.31972...
#define VDIV_SCALE_FACTOR     0.3197f
#define ADC_VREF              5.0f    // Arduino Nano ADC reference voltage
#define ADC_RESOLUTION        1024.0f // 10-bit ADC

// ─── LM35 Calibration ────────────────────────────────────────────────────────
// LM35 output: 10mV/°C. At 5V ADC ref: 1 ADC step = 4.88mV = 0.488°C
#define LM35_MV_PER_C         10.0f  // mV per degree Celsius

// ─── Payment Configuration ───────────────────────────────────────────────────
#define RATE_PER_UNIT_RS      10.0f  // Rupees per kWh (1 unit)
#define MIN_PAYMENT_RS        10.0f  // Minimum accepted payment in Rupees
#define MAX_PAYMENT_RS        500.0f // Maximum single payment cap

// ─── GSM Configuration ────────────────────────────────────────────────────────
#define GSM_BAUD_RATE         9600
#define GSM_INIT_TIMEOUT_MS   5000
#define GSM_CMD_TIMEOUT_MS    2000
#define SMS_POLL_INTERVAL_MS  5000   // Poll for new SMS every 5 seconds
#define GSM_RESPONSE_BUF      512    // SMS response buffer size (bytes)

// ─── Timing Constants ─────────────────────────────────────────────────────────
#define SENSOR_POLL_INTERVAL_MS   2000   // Read sensors every 2 seconds
#define LCD_UPDATE_INTERVAL_MS    2000   // Refresh LCD every 2 seconds
#define LCD_BILL_DISPLAY_MS      10000   // Show session summary for 10 seconds
#define FAULT_RETRY_INTERVAL_MS  30000   // Retry after fault every 30 seconds

// ─── Buzzer Durations ────────────────────────────────────────────────────────
#define BEEP_SHORT_MS         100
#define BEEP_LONG_MS          500

// ─── EEPROM Layout ───────────────────────────────────────────────────────────
#define EEPROM_MAGIC_BYTE     0xEA   // Written at address 0 to validate EEPROM
#define EEPROM_RECORD_START   4      // Session records start at byte 4
#define EEPROM_RECORD_SIZE    11     // bytes per session record
#define EEPROM_MAX_RECORDS    90     // (1024 - 4) / 11 = 92, use 90 for safety

#endif // CONFIG_H
