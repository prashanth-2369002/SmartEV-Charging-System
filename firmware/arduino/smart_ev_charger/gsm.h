/*
 * gsm.h — GSM SIM900A driver for Smart EV Charging Station
 * Platform: Arduino Nano (ATmega328P)
 */

#ifndef GSM_H
#define GSM_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"

struct PaymentInfo {
    bool  isValid;
    float amount;
    char  senderHint[16];  // Last few digits of sender, for logging
};

class GSM {
public:
    GSM(uint8_t rxPin, uint8_t txPin);
    void begin();
    bool isReady() const;

    // Poll for incoming payment SMS — returns PaymentInfo with isValid = true if found
    PaymentInfo pollForPayment();

    // Send a reply SMS (optional confirmation to user)
    bool sendSMS(const char* number, const char* message);

    // Delete all read SMS messages (prevent re-triggering)
    void deleteAllSMS();

    // Get last error description
    const char* getLastError() const;

private:
    SoftwareSerial _serial;
    bool           _ready;
    char           _lastError[64];

    bool    sendAT(const char* cmd, const char* expected, uint16_t timeoutMs);
    String  sendATResponse(const char* cmd, uint16_t timeoutMs);
    float   extractAmount(const String& text);
    bool    hasPaymentKeywords(const String& text);
    bool    hasRejectionKeywords(const String& text);
};

#endif // GSM_H
