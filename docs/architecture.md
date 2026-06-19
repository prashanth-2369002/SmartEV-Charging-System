# System Architecture — Smart EV Charging Station with BMS

## Overview

The system is built around an **Arduino Nano (ATmega328P)** as the central controller, orchestrating four subsystems: GSM payment verification, battery monitoring, protection control, and user feedback. The architecture is deliberately lean — all logic fits within 32KB of flash and 2KB of SRAM.

---

## Hardware Architecture Layers

```mermaid
graph TD
    subgraph LAYER1 ["LAYER 1 — USER INTERACTION"]
        U[EV User]
        PHONE[User Phone - UPI App]
        LCD[16x2 LCD Display]
    end

    subgraph LAYER2 ["LAYER 2 — COMMUNICATION"]
        GSM[GSM SIM900A - Quad-band 2G]
    end

    subgraph LAYER3 ["LAYER 3 — CONTROLLER"]
        MCU[Arduino Nano - ATmega328P - 32KB Flash / 2KB SRAM]
    end

    subgraph LAYER4 ["LAYER 4 — SENSING"]
        TEMP[LM35 Temperature Sensor]
        VDIV[Voltage Divider 10k + 4.7k]
    end

    subgraph LAYER5 ["LAYER 5 — PROTECTION AND ACTUATION"]
        BMS[BMS Module - 3S Protection IC]
        SSR[Solid State Relay - 24V / 10A]
    end

    subgraph LAYER6 ["LAYER 6 — ENERGY STORAGE"]
        BATT[3S Li-ion Pack - 18650 Cells - 11.1V nominal]
    end

    PHONE -->|UPI SMS| GSM
    GSM -->|UART AT Commands| MCU
    MCU -->|GPIO| LCD
    TEMP -->|Analog mV| MCU
    VDIV -->|Scaled Voltage| MCU
    MCU -->|Digital Control| SSR
    MCU -->|Enable Signal| BMS
    BMS -->|Hardware Cutoff| SSR
    SSR -->|Charge Current| BATT
    BATT -->|Pack Voltage| VDIV
    U -->|Physical Interaction| LCD
```

---

## Controller Architecture (Arduino Nano)

```mermaid
graph LR
    subgraph MCU ["ATmega328P Internal"]
        ADC[10-bit ADC - 6 channels]
        UART[UART - Hardware + Software]
        GPIO[Digital GPIO - 22 pins]
        EEPROM[EEPROM - 1024 bytes]
        TIMER[Timers - T0/T1/T2]
    end

    ADC --> VREAD[Voltage Reading - A1 Pack Voltage]
    ADC --> TREAD[Temperature Reading - A0 LM35]
    UART --> GSM_IF[SoftwareSerial D12/D13 to SIM900A]
    GPIO --> LCD_IF[LCD Interface D2-D7 to HD44780]
    GPIO --> SSR_IF[SSR Control D8 to Relay IN]
    GPIO --> LED_IF[Status LEDs D9 Green / D10 Red]
    GPIO --> BUZ_IF[Buzzer D11]
    EEPROM --> SESSLOG[Session Logger - 50 records max]
    TIMER --> MILLIS[millis based non-blocking timing]
```

---

## Software Architecture

```mermaid
graph TD
    MAIN[main loop] --> GSM_POLL[GSM Poller - Check SMS every 5 sec]
    MAIN --> SENSOR_POLL[Sensor Poller - Read ADC every 2 sec]
    MAIN --> LCD_UPDATE[LCD Updater - Refresh every 2 sec]
    MAIN --> STATE_MACHINE[State Machine - Evaluate transitions]

    GSM_POLL --> SMS_PARSE[SMS Parser - Extract payment keywords]
    SENSOR_POLL --> VOLT_CALC[Voltage Calculator - ADC to Pack Voltage]
    SENSOR_POLL --> TEMP_CALC[Temperature Calculator - ADC to Celsius]
    SMS_PARSE --> PAYMENT_VERIFY[Payment Verifier - Validate amount]

    STATE_MACHINE --> IDLE_ST[IDLE State]
    STATE_MACHINE --> PAY_ST[PAYMENT_PENDING State]
    STATE_MACHINE --> CHARGE_ST[CHARGING State]
    STATE_MACHINE --> FAULT_ST[FAULT State]
    STATE_MACHINE --> END_ST[SESSION_END State]

    CHARGE_ST --> SAFETY_CHECK[Safety Monitor - Voltage + Temp + BMS]
    SAFETY_CHECK -->|Fault| FAULT_ST
    SAFETY_CHECK -->|Full| END_ST
    SAFETY_CHECK -->|OK| CHARGE_ST
```

---

## Memory Layout (ATmega328P)

| Region | Size | Used For |
|---|---|---|
| Flash | 32 KB | Program code + string constants |
| SRAM | 2 KB | Runtime variables, LCD buffer, SMS buffer |
| EEPROM | 1 KB | Session records (50 × ~20 bytes) |
| Bootloader | 2 KB | Arduino bootloader (reserved) |

**Memory optimization decisions:**
- SoftwareSerial used instead of hardware serial to free hardware UART for debugging
- LCD strings stored in `PROGMEM` to reduce SRAM usage
- SMS receive buffer limited to 160 characters (single SMS length)
- No dynamic memory allocation — all buffers are statically declared

---

## Communication Protocols

| Interface | Protocol | Peripheral | Speed |
|---|---|---|---|
| Arduino ↔ GSM SIM900A | UART (SoftwareSerial) | SIM900A | 9600 baud |
| Arduino ↔ LCD | Parallel 4-bit | HD44780 | GPIO toggle |
| Arduino ↔ LM35 | Analog | LM35 | ADC sampling |
| Arduino ↔ Voltage Divider | Analog | Resistors | ADC sampling |
| Arduino ↔ SSR | Digital GPIO | SSR | HIGH/LOW |
| Arduino ↔ BMS | Digital GPIO (enable) | BMS IC | HIGH/LOW |

---

## Design Decisions

| Decision | Rationale |
|---|---|
| Arduino Nano over UNO | Smaller form factor for compact prototype |
| SoftwareSerial for GSM | Frees hardware UART for debug Serial monitor |
| LM35 over DS18B20 | No external library needed; direct ADC reading |
| Voltage divider over INA219 | Lower cost; sufficient accuracy for 3S Li-ion display |
| Commercial BMS module | Hardware protection more reliable than software-only |
| SSR over mechanical relay | Silent operation; faster switching; no contact wear |
| 16×2 LCD over OLED | Lower cost; adequate for field data display |
| EEPROM logging | Persistent session data across power cycles |
