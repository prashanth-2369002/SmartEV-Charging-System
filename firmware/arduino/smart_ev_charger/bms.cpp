/*
 * bms.cpp — Battery monitoring and SOC estimation implementation
 * Platform: Arduino Nano (ATmega328P)
 */

#include "bms.h"

BMS::BMS() : _activeFault(BatteryFault::NONE), _tempIdx(0) {
    for (int i = 0; i < 4; i++) _tempBuf[i] = 25.0f;
}

void BMS::begin() {
    pinMode(PIN_TEMP_ADC, INPUT);
    pinMode(PIN_VOLT_ADC, INPUT);
    _activeFault = BatteryFault::NONE;
}

BatteryData BMS::read() {
    BatteryData data;
    data.packVoltage   = readPackVoltage();
    data.temperatureC  = readTemperature();
    data.socPercent    = estimateSOC(data.packVoltage);
    data.fault         = checkFaults(data.packVoltage, data.temperatureC);
    _activeFault       = data.fault;
    return data;
}

void BMS::startSession() {
    _activeFault = BatteryFault::NONE;
}

void BMS::endSession() {
    // Intentionally empty — session state managed externally
}

bool BMS::isFaultActive() const {
    return _activeFault != BatteryFault::NONE;
}

BatteryFault BMS::getFaultType() const {
    return _activeFault;
}

void BMS::clearFault() {
    _activeFault = BatteryFault::NONE;
}

// ─── Private ──────────────────────────────────────────────────────────────────

float BMS::readPackVoltage() {
    int raw = analogRead(PIN_VOLT_ADC);
    float adcVoltage = raw * (ADC_VREF / ADC_RESOLUTION);
    return adcVoltage / VDIV_SCALE_FACTOR;  // Reverse voltage divider
}

float BMS::readTemperature() {
    int raw = analogRead(PIN_TEMP_ADC);
    float adcVoltage = raw * (ADC_VREF / ADC_RESOLUTION);  // 0–5V
    float tempC = adcVoltage * 100.0f;  // LM35: 10mV/°C → V * 100 = °C

    // Running average over 4 samples to reject noise
    _tempBuf[_tempIdx % 4] = tempC;
    _tempIdx++;
    float sum = 0.0f;
    for (int i = 0; i < 4; i++) sum += _tempBuf[i];
    return sum / 4.0f;
}

float BMS::estimateSOC(float packVoltage) {
    // Voltage-to-SOC lookup table for 3S Li-ion (18650)
    // Based on typical Li-ion discharge curve at 0.2C
    if (packVoltage >= 12.60f) return 100.0f;
    if (packVoltage >= 12.45f) return  95.0f;
    if (packVoltage >= 12.30f) return  90.0f;
    if (packVoltage >= 12.15f) return  82.5f;
    if (packVoltage >= 12.00f) return  75.0f;
    if (packVoltage >= 11.85f) return  67.5f;
    if (packVoltage >= 11.70f) return  60.0f;
    if (packVoltage >= 11.55f) return  52.5f;
    if (packVoltage >= 11.40f) return  45.0f;
    if (packVoltage >= 11.25f) return  37.5f;
    if (packVoltage >= 11.10f) return  30.0f;
    if (packVoltage >= 10.95f) return  22.5f;
    if (packVoltage >= 10.80f) return  15.0f;
    if (packVoltage >= 10.50f) return  10.0f;
    if (packVoltage >=  9.00f) return   5.0f;
    return 0.0f;
}

BatteryFault BMS::checkFaults(float voltage, float temp) {
    if (voltage >= VOLT_OVERVOLT_SW)  return BatteryFault::OVERVOLTAGE;
    if (temp    >= TEMP_CUTOFF_C)     return BatteryFault::OVERTEMP;
    return BatteryFault::NONE;
}
