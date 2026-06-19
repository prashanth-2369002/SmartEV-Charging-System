# Working Principle — Smart EV Charging Station with BMS

## Overview

The Smart EV Charging Station operates as a five-stage process: system initialization, payment verification via GSM SMS, pre-charge safety validation, active charging with continuous monitoring, and session termination. Each stage is controlled by the Arduino Nano state machine.

---

## Stage 1 — System Initialization

On power-on, the Arduino Nano:
1. Initializes all GPIO pins (SSR, LEDs, buzzer as outputs; ADC channels as inputs)
2. Starts the 16×2 LCD and displays "Initializing..."
3. Initializes SoftwareSerial for GSM SIM900A communication at 9600 baud
4. Sends AT commands to verify GSM module is responsive (`AT` → expect `OK`)
5. Configures GSM for SMS text mode: `AT+CMGF=1`
6. Reads LM35 temperature as baseline
7. Reads voltage divider ADC for initial battery voltage
8. Transitions to IDLE state; LCD displays "Awaiting Payment"

---

## Stage 2 — Payment Verification (GSM SMS Flow)

This is the core differentiator. The system does not require internet connectivity — it uses GSM 2G SMS:

1. **User initiates payment:** The EV user opens any UPI app (Google Pay, PhonePe, Paytm) and sends a payment to the station's registered mobile number.
2. **SMS delivery:** The UPI app's bank sends an SMS confirmation to the station SIM: `"Payment of Rs 50 received from XXXX@upi. Reference: 1234567890."`
3. **Arduino polls GSM:** Every 5 seconds, the Arduino sends `AT+CMGL="ALL"` to SIM900A and reads the response buffer.
4. **SMS parsing:** The response is searched for payment-indicative keywords: `"received"`, `"credited"`, `"Payment"`, `"Rs"`. The amount is extracted using string parsing.
5. **Validation:** Amount is compared against the minimum configured payment threshold (e.g., ₹10 minimum).
6. **Authorization:** If valid, charging duration is calculated: `duration_min = (amount / RATE_PER_UNIT) × 60`
7. **Cleanup:** Processed SMS is deleted with `AT+CMGD=1,4` to prevent re-triggering.

---

## Stage 3 — Pre-Charge Safety Check

Before the SSR is activated, the controller runs a safety preflight:

```
IF battery voltage > VOLT_MAX_THRESHOLD → FAULT (Overvoltage)
IF battery temperature > TEMP_MAX_THRESHOLD → FAULT (Overtemperature)
IF BMS fault signal active → FAULT (BMS hardware fault)
ELSE → Proceed to charging
```

This ensures the SSR only closes when all conditions are safe. The BMS module provides a secondary hardware-level check independent of the Arduino software.

---

## Stage 4 — Active Charging with Continuous Monitoring

Once the SSR activates:

1. **Charging begins:** SSR HIGH → current flows from supply through BMS to battery pack.
2. **Sensor polling (every 2 seconds):**
   - LM35: `temperature_C = (analogRead(A0) × 5.0 / 1024.0) × 100`
   - Voltage: `pack_voltage = analogRead(A1) × (5.0 / 1024.0) × VOLTAGE_DIVIDER_RATIO`
3. **SOC estimation:** Pack voltage is mapped to SOC% using a Li-ion voltage-SOC lookup table (voltage curve approximation for 3S configuration).
4. **LCD update:** SOC%, voltage, temperature, and elapsed time are refreshed.
5. **Safety monitoring:** Every cycle checks voltage and temperature against thresholds. On fault: SSR immediately goes LOW, buzzer sounds, LCD shows fault type.
6. **Session timer:** Tracks elapsed seconds. When paid duration is exhausted, session ends automatically.

---

## Stage 5 — Session Termination

Session ends when any of these conditions is true:
- Battery voltage reaches full charge threshold (4.20V/cell = 12.6V for 3S)
- Estimated SOC reaches 100%
- Pre-paid session duration elapses
- A safety fault is triggered

On termination:
1. SSR LOW (charging stops immediately)
2. Session summary calculated: energy ≈ `power_avg × time_hours`
3. LCD displays: "Session Complete / Energy: X.X Wh / Duration: XX min"
4. Record saved to EEPROM
5. System returns to IDLE

---

## Charging Algorithm Detail

```
// SOC estimation via voltage-SOC lookup table (3S Li-ion)
float voltToSOC(float v) {
    // Voltage (3S pack) → SOC mapping
    if (v >= 12.60) return 100.0;
    if (v >= 12.30) return 90.0;
    if (v >= 12.00) return 75.0;
    if (v >= 11.70) return 60.0;
    if (v >= 11.40) return 45.0;
    if (v >= 11.10) return 30.0;
    if (v >= 10.80) return 15.0;
    if (v >= 9.00)  return 5.0;
    return 0.0;
}
```

The lookup table approach is used because coulomb counting requires current sensing hardware not included in this prototype. For a 3S Li-ion pack, the voltage-SOC relationship is reasonably predictable.

---

## Failure Scenarios

| Failure | Detection | Response |
|---|---|---|
| GSM module not responding | `AT` command returns no `OK` within timeout | LCD: "GSM Fail"; System stays IDLE; retries every 30 sec |
| SMS not received | No payment SMS after 5 minutes | No action (system stays IDLE) |
| Overtemperature during charge | LM35 reading > `TEMP_CUTOFF_C` | SSR OFF immediately; LCD: "TEMP FAULT"; buzzer; wait for cool-down |
| Overvoltage | Pack voltage > `VOLT_MAX` | SSR OFF; LCD: "OV FAULT"; BMS hardware also trips |
| BMS hardware trip | BMS disconnects load externally | SSR already off (BMS acts independently of Arduino) |
| Power interruption | — | EEPROM retains last session log; LCD re-initializes on power restore |
| SMS parse failure | Keywords not found in SMS text | SMS deleted; LCD: "Invalid Payment"; system stays IDLE |

---

## GSM AT Command Sequence

```
// Initialization
AT                    → OK
AT+CMGF=1             → OK   (set SMS text mode)
AT+CNMI=2,2,0,0,0     → OK   (route new SMS to serial immediately, optional)

// Poll for messages
AT+CMGL="ALL"         → +CMGL: 1,"REC READ","XXXXXXXX",,"26/06/19,10:30:00+22"
                         "Payment of Rs 50 received from user@upi..."
                         OK

// Delete processed message
AT+CMGD=1,4           → OK   (delete all read messages)

// Send reply SMS (optional)
AT+CMGS="+91XXXXXXXXXX"  → >  (type message)
Charging started. Station EV-01\x1A  (Ctrl+Z to send)
                       → +CMGS: 1
                         OK
```

---

## Power Budget

| Component | Typical Current | Notes |
|---|---|---|
| Arduino Nano | ~20 mA | Running at 16MHz, all peripherals active |
| LCD (backlight ON) | ~20 mA | With backlight; ~5mA without |
| LM35 | ~0.06 mA | Negligible |
| GSM SIM900A (idle) | ~10 mA | Spikes to 2A during TX burst |
| GSM SIM900A (TX) | Up to 2A peak | Requires dedicated 4V / 2A supply |
| SSR (control coil) | ~15 mA | 5V control signal |
| LEDs | ~10 mA total | Two status LEDs |
| **Total (controller side)** | **~75 mA steady** | Excluding GSM TX burst |

> **Critical:** GSM SIM900A must be powered from a **separate 4V / 2A supply** — not from the Arduino 5V pin, which cannot deliver the 2A burst current required during SMS transmission.
