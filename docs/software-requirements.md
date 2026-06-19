# Software Requirements — Smart EV Charging Station with BMS

## Development Environment

| Tool | Version | Platform | Purpose |
|---|---|---|---|
| Arduino IDE | 2.3.x or later | Windows / Linux / macOS | Firmware development and upload |
| Arduino Nano Board Package | ATmega328P (built-in) | Arduino IDE | Board definitions |
| avr-gcc | Bundled with Arduino IDE | — | C++ compiler for ATmega328P |
| avrdude | Bundled with Arduino IDE | — | Firmware upload via USB |
| Git | 2.x | All platforms | Version control |
| Serial Monitor | Arduino IDE built-in | — | Debug output from Serial.print |

---

## Arduino Libraries Required

All libraries below are installable via Arduino IDE → **Tools → Manage Libraries**.

| Library | Version | Purpose | Install Name |
|---|---|---|---|
| LiquidCrystal | Built-in (Arduino) | 16×2 LCD (HD44780) driver | `LiquidCrystal` |
| SoftwareSerial | Built-in (Arduino) | UART for GSM SIM900A | `SoftwareSerial` |
| EEPROM | Built-in (Arduino) | Session logging to EEPROM | `EEPROM` |

**No third-party libraries required.** The firmware intentionally uses only Arduino built-in libraries to minimize flash usage and avoid dependency issues on constrained hardware (32KB flash).

---

## Arduino IDE Setup

### 1. Install Arduino IDE

Download Arduino IDE 2.3.x from [arduino.cc/en/software](https://www.arduino.cc/en/software).

### 2. Board Configuration

Open the firmware sketch and set:

| Setting | Value |
|---|---|
| Board | `Arduino Nano` |
| Processor | `ATmega328P` (or `ATmega328P Old Bootloader` if upload fails) |
| Port | COM port of your Arduino Nano (varies by system) |
| Upload Speed | `115200` |

### 3. Open the Sketch

```
File → Open → firmware/arduino/smart_ev_charger/smart_ev_charger.ino
```

Arduino IDE will prompt to open the full sketch folder — accept this so all `.h` and `.cpp` files are included.

### 4. Calibrate `config.h`

Before uploading, open `config.h` and review:

```cpp
// Verify your voltage divider resistor values match these
#define VDIV_SCALE_FACTOR     0.3197f  // R2/(R1+R2) = 4700/14700

// Set your UPI payment rate
#define RATE_PER_UNIT_RS      10.0f    // ₹ per kWh

// Adjust safety thresholds if needed
#define TEMP_CUTOFF_C         45.0f    // °C — overtemp cutoff
```

### 5. Compile and Upload

Click **Verify** (✓) first to check for compilation errors, then **Upload** (→).

Expected compile output:
```
Sketch uses ~21000 bytes (68%) of program storage space.
Global variables use ~450 bytes (21%) of dynamic memory.
```

---

## Hardware Driver Requirements

| Component | Driver | Notes |
|---|---|---|
| Arduino Nano (CH340G USB chip) | CH340 driver | Required on Windows 10/11 if not auto-installed. Download from [wch-ic.com](http://www.wch-ic.com/downloads/CH341SER_EXE.html) |
| Arduino Nano (CP2102 USB chip) | CP2102 driver | Some clone Nanos use CP2102. Download from Silicon Labs website |
| Arduino Nano (FTDI USB chip) | FTDI VCP driver | Genuine Arduino Nanos use FTDI. Usually auto-installed on Windows |

**To identify your USB chip:** Check the small IC between the USB port and the Nano's edge. CH340G = clone, FT232RL = genuine.

---

## Serial Monitor Usage

Connect Arduino via USB and open **Tools → Serial Monitor** at **115200 baud** to see debug output:

```
[BOOT] Smart EV Charger v1.0
[BMS] Initial voltage: 11.24V  SOC: 45%  Temp: 28.4C
[GSM] Sending AT...
[GSM] Module OK
[GSM] SMS text mode set
[GSM] Ready
[SYSTEM] State: IDLE
[GSM] Polling for SMS...
[GSM] No messages
[GSM] Polling for SMS...
[GSM] Found 1 message
[GSM] SMS body: "Payment of Rs 50 received..."
[GSM] Payment valid: Rs 50.00
[GSM] Session duration: 18000 seconds
[SYSTEM] State: PRE_CHECK
[BMS] Voltage: 11.24V  Temp: 28.4C  SOC: 45%  Fault: NONE
[SYSTEM] Safety OK — starting charge
[SYSTEM] State: CHARGING
[BMS] Voltage: 11.31V  Temp: 29.1C  SOC: 47%
[BMS] Voltage: 11.38V  Temp: 29.3C  SOC: 50%
```

---

## Verifying Hardware Connections via Serial Monitor

Before final assembly, use these quick tests:

**Test 1 — Voltage divider reading:**
```cpp
// Add to setup() temporarily:
Serial.println(analogRead(A1));  // Should be ~460–860 for 9–16.8V range
```

**Test 2 — LM35 temperature reading:**
```cpp
// Add to setup() temporarily:
float temp = analogRead(A0) * (5.0 / 1024.0) * 100.0;
Serial.print("Temp: "); Serial.println(temp);  // Should read room temp ~25-35°C
```

**Test 3 — GSM AT response:**
```cpp
// Add to setup() temporarily:
gsmSerial.println("AT");
delay(1000);
while (gsmSerial.available()) Serial.write(gsmSerial.read());
// Expected output: "AT\r\n\r\nOK\r\n"
```

**Test 4 — SSR control:**
```cpp
// Add to loop() temporarily:
digitalWrite(PIN_SSR, HIGH); delay(2000);
digitalWrite(PIN_SSR, LOW);  delay(2000);
// SSR should click/activate every 2 seconds
```

---

## Repository Clone and Setup

```bash
git clone https://github.com/prashanth-2369002/SmartEV-Charging-System.git
cd SmartEV-Charging-System

# Open firmware in Arduino IDE
# File → Open → firmware/arduino/smart_ev_charger/smart_ev_charger.ino
```

No `pip install` or `npm install` required — this is a pure Arduino (C++) embedded project with no host-side runtime dependencies.
