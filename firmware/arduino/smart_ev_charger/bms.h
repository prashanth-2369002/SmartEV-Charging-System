/*
 * bms.h — Battery monitoring and SOC estimation
 * Platform: Arduino Nano (ATmega328P)
 * Sensors: LM35 (temperature), Voltage divider (pack voltage)
 */

#ifndef BMS_H
#define BMS_H

#include <Arduino.h>
#include "config.h"

enum class BatteryFault : uint8_t {
    NONE        = 0,
    OVERVOLTAGE = 1,
    OVERTEMP    = 2,
    UNDERTEMP   = 3
};

struct BatteryData {
    float        packVoltage;    // V — actual pack voltage (3S)
    float        temperatureC;   // °C — cell temperature from LM35
    float        socPercent;     // % — estimated state of charge
    BatteryFault fault;          // Fault type, NONE if safe
    unsigned long sessionStartMs; // millis() when session started
};

class BMS {
public:
    BMS();
    void begin();

    BatteryData read();

    void startSession();
    void endSession();

    bool isFaultActive() const;
    BatteryFault getFaultType() const;
    void clearFault();

private:
    BatteryFault _activeFault;

    // Temperature smoothing (running average, N=4)
    float _tempBuf[4];
    uint8_t _tempIdx;

    float readPackVoltage();
    float readTemperature();
    float estimateSOC(float packVoltage);
    BatteryFault checkFaults(float voltage, float temp);
};

#endif // BMS_H
