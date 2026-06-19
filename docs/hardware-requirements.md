# Hardware Design — Smart EV Charging Station with BMS

## Bill of Materials (BOM)

| # | Component | Specification | Qty | Est. Cost (INR) | Purpose |
|---|---|---|---|---|---|
| 1 | Arduino Nano | ATmega328P, 32KB Flash, 2KB SRAM, 16MHz | 1 | ₹250 | Main controller |
| 2 | GSM Module SIM900A | Quad-band 850/900/1800/1900 MHz, UART, 4V supply | 1 | ₹700 | SMS payment verification |
| 3 | BMS Module (3S) | 11.1V Li-ion, 10A, overcurrent + overvoltage + short circuit protection | 1 | ₹150 | Battery protection |
| 4 | Solid State Relay | 5V control input, 24V DC / 10A output | 1 | ₹300 | Silent charging circuit control |
| 5 | LM35 Temperature Sensor | 10mV/°C, −55°C to +150°C, TO-92 package | 1 | ₹30 | Cell temperature monitoring |
| 6 | LCD 16×2 | HD44780 compatible, 5V, blue/green backlight | 1 | ₹80 | User status display |
| 7 | 10kΩ Resistors | ¼W, 5% tolerance | 4 | ₹5 | Voltage divider (upper leg) + pull-ups |
| 8 | 4.7kΩ Resistors | ¼W, 5% tolerance | 2 | ₹5 | Voltage divider (lower leg) |
| 9 | 100nF Ceramic Capacitors | 0.1µF, 50V | 4 | ₹10 | Decoupling caps |
| 10 | 10µF Electrolytic Capacitors | 16V | 2 | ₹10 | Power supply filtering |
| 11 | 7805 Voltage Regulator | +5V, 1A, TO-220 package | 1 | ₹15 | 5V rail for Arduino and LCD |
| 12 | 18650 Li-ion Cells | 3S configuration, ~11.1V nominal, 2000mAh | 3 | ₹600 | Simulated EV battery pack |
| 13 | 12V 2A DC Adapter | Input: 230V AC, Output: 12V DC | 1 | ₹200 | Main power supply |
| 14 | LED (Green, Red) | 5mm, through-hole | 2 | ₹10 | Status indicators |
| 15 | Active Buzzer | 5V, 85dB, through-hole | 1 | ₹20 | Audio alerts |
| 16 | SIM Card | Any 2G-enabled SIM (Jio/Airtel/BSNL) | 1 | ₹10 | GSM connectivity |
| 17 | Breadboard | 830-point, full size | 1 | ₹100 | Prototyping |
| 18 | Jumper Wires | M-M, M-F, F-F, 20cm | 1 set | ₹80 | Connections |
| 19 | USB-A to Mini-USB cable | For Arduino programming | 1 | ₹50 | Firmware upload |
| | **TOTAL** | | | **~₹2,625** | |

---

## Circuit Description

### Power Supply Circuit

```
230V AC → 12V DC Adapter → 7805 Regulator → 5V Rail → Arduino Nano + LCD
                                          ↓
                         4V Regulator / Buck Converter → GSM SIM900A
```

The SIM900A **must not** be powered from the Arduino's 5V pin. The module draws up to 2A during GSM transmission, which will cause the Arduino to reset. A separate 4V supply (from a dedicated buck converter or a LiPo cell) is required.

### Voltage Divider Circuit (Battery Voltage Sensing)

The 3S Li-ion pack voltage ranges from 9.0V (discharged) to 12.6V (fully charged). The Arduino ADC operates at 0–5V. A voltage divider scales the pack voltage to the ADC range:

```
Pack+ ─── R1 (10kΩ) ─── A1 ─── R2 (4.7kΩ) ─── GND

Scaling factor = R2 / (R1 + R2) = 4700 / 14700 = 0.3197

Max ADC input at full charge = 12.6V × 0.3197 = 4.03V  (safely within 0–5V range)
Min ADC input at discharge   = 9.0V  × 0.3197 = 2.88V

ADC to voltage formula:
  adc_val = analogRead(A1);
  volt_divider_output = adc_val × (5.0 / 1024.0);
  pack_voltage = volt_divider_output / 0.3197;
```

### LM35 Temperature Sensing

LM35 outputs 10mV per °C, linear. Connected directly to Arduino A0:

```
VCC (5V) → LM35 pin 1
GND       → LM35 pin 3
Output    → A0

temperature_C = (analogRead(A0) × 5.0 / 1024.0) × 100.0;
```

At 25°C: Output = 250mV → ADC = 51 → temp = 25.0°C.

### SSR Control

The SSR control signal is driven from Arduino D8:

```
D8 HIGH (5V) → SSR internal LED conducts → Triac/MOSFET turns ON → charging circuit closes
D8 LOW (0V)  → SSR OFF → charging circuit opens
```

SSR provides galvanic isolation between the Arduino's 5V logic and the battery charging circuit.

### GSM SIM900A UART Connection

```
Arduino D0 (RX via SoftwareSerial) → SIM900A TX
Arduino D1 (TX via SoftwareSerial) → SIM900A RX
SIM900A GND → Common GND
SIM900A VCC → Dedicated 4V supply (separate from Arduino supply)
```

> **Note:** SoftwareSerial is used (pins D0/D1 in software, not hardware UART), so the hardware UART (USB) remains free for Serial.print() debugging during development.

---

## Arduino Nano Pin Assignment

| Pin | Label | Direction | Connected To | Notes |
|---|---|---|---|---|
| D0 | RX (SW Serial) | IN | GSM SIM900A TX | SoftwareSerial |
| D1 | TX (SW Serial) | OUT | GSM SIM900A RX | SoftwareSerial |
| D2 | LCD RS | OUT | LCD pin 4 | Register Select |
| D3 | LCD EN | OUT | LCD pin 6 | Enable |
| D4 | LCD D4 | OUT | LCD pin 11 | Data bit 4 |
| D5 | LCD D5 | OUT | LCD pin 12 | Data bit 5 |
| D6 | LCD D6 | OUT | LCD pin 13 | Data bit 6 |
| D7 | LCD D7 | OUT | LCD pin 14 | Data bit 7 |
| D8 | SSR_CTRL | OUT | SSR IN | HIGH = Relay ON |
| D9 | LED_GREEN | OUT | Green LED + 220Ω | Session active indicator |
| D10 | LED_RED | OUT | Red LED + 220Ω | Fault indicator |
| D11 | BUZZER | OUT | Active buzzer | Audio alert |
| A0 | TEMP_ADC | IN | LM35 output | Temperature reading |
| A1 | VOLT_ADC | IN | Voltage divider output | Battery voltage |
| 5V | VCC | PWR | Arduino onboard regulator | Powers LCD, LM35, SSR |
| GND | GND | PWR | Common ground | All modules |

---

## BMS Module Details

The commercial 3S BMS module provides **hardware-level** protection independent of Arduino firmware:

| Protection | Threshold | Action |
|---|---|---|
| Overcharge protection | 4.20V per cell (12.60V total) | Disconnects charge MOSFET |
| Over-discharge protection | 2.75V per cell (8.25V total) | Disconnects discharge MOSFET |
| Overcurrent protection | Typically 10A (module specific) | Disconnects load |
| Short circuit protection | Instantaneous (microseconds) | Disconnects immediately |
| Cell balancing | Passive balancing via bypass resistors | Equalizes cell voltages |

The BMS module sits **between the Arduino-controlled SSR and the battery pack** in the charging current path, providing a fail-safe even if the Arduino firmware fails to act.

---

## Prototype Assembly Notes

1. Wire all GND connections first (common ground for Arduino, LCD, LM35, voltage divider, BMS, SSR)
2. Power SIM900A from dedicated supply — connect after everything else is verified
3. Verify voltage divider output at A1 with multimeter before powering Arduino
4. Test LM35 reading first with a Serial.print before connecting LCD
5. Test SSR control with an LED before connecting battery pack
6. Insert SIM card before powering SIM900A
7. Power sequence: Arduino → LCD → LM35 → Voltage divider → SSR → BMS → GSM

---

## Circuit Schematic

See `images/diagrams/` for schematic diagram images. Key connections:

```
[12V Adapter] → [7805 Regulator] → 5V → [Arduino Nano] → [LCD, LEDs, Buzzer, SSR]
                                       → [LM35 A0]
                                       → [Voltage Divider A1]
[Separate 4V Supply] → [SIM900A VCC]
[SIM900A TX/RX] ←→ [Arduino D0/D1 SoftwareSerial]
[Arduino D8] → [SSR IN] → [SSR OUT] → [Battery Pack Positive]
[Battery Pack] → [BMS Module] → [SSR load side]
```
