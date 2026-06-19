/*
 * gsm.cpp — GSM SIM900A driver implementation
 * Platform: Arduino Nano (ATmega328P)
 */

#include "gsm.h"

GSM::GSM(uint8_t rxPin, uint8_t txPin)
    : _serial(rxPin, txPin), _ready(false)
{
    _lastError[0] = '\0';
}

void GSM::begin() {
    _serial.begin(GSM_BAUD_RATE);
    delay(2000);  // Allow SIM900A to boot

    if (!sendAT("AT", "OK", GSM_INIT_TIMEOUT_MS)) {
        strncpy(_lastError, "Module not responding", sizeof(_lastError));
        return;
    }
    if (!sendAT("AT+CMGF=1", "OK", GSM_CMD_TIMEOUT_MS)) {
        strncpy(_lastError, "SMS text mode failed", sizeof(_lastError));
        return;
    }
    // Disable auto-delivery notification (use polling instead)
    sendAT("AT+CNMI=0,0,0,0,0", "OK", GSM_CMD_TIMEOUT_MS);

    _ready = true;
}

bool GSM::isReady() const {
    return _ready;
}

const char* GSM::getLastError() const {
    return _lastError;
}

PaymentInfo GSM::pollForPayment() {
    PaymentInfo result = {false, 0.0f, ""};
    if (!_ready) return result;

    String response = sendATResponse("AT+CMGL=\"ALL\"", 3000);

    if (response.indexOf("+CMGL:") == -1) return result;  // No messages

    // Extract the SMS body text (everything after the header line)
    int bodyStart = response.indexOf('\n', response.indexOf("+CMGL:"));
    if (bodyStart == -1) return result;
    String smsBody = response.substring(bodyStart + 1);
    smsBody.trim();

    if (!hasPaymentKeywords(smsBody))  return result;
    if (hasRejectionKeywords(smsBody)) return result;

    float amount = extractAmount(smsBody);
    if (amount < MIN_PAYMENT_RS || amount > MAX_PAYMENT_RS) return result;

    result.isValid = true;
    result.amount  = amount;

    // Extract last 4 digits of sender phone (for log)
    int senderIdx = response.indexOf("+CMGL:") + 20;
    String senderField = response.substring(senderIdx, senderIdx + 15);
    senderField.trim();
    int len = senderField.length();
    strncpy(result.senderHint,
            senderField.substring(len > 4 ? len - 4 : 0).c_str(),
            sizeof(result.senderHint));

    return result;
}

bool GSM::sendSMS(const char* number, const char* message) {
    if (!_ready) return false;

    _serial.print(F("AT+CMGS=\""));
    _serial.print(number);
    _serial.println(F("\""));
    delay(100);

    if (_serial.find(">")) {
        _serial.print(message);
        _serial.write(0x1A);  // Ctrl+Z terminates message
        delay(3000);
        return true;
    }
    return false;
}

void GSM::deleteAllSMS() {
    sendAT("AT+CMGD=1,4", "OK", 3000);
}

// ─── Private helpers ──────────────────────────────────────────────────────────

bool GSM::sendAT(const char* cmd, const char* expected, uint16_t timeoutMs) {
    _serial.println(cmd);
    unsigned long start = millis();
    String resp = "";
    while (millis() - start < timeoutMs) {
        while (_serial.available()) resp += (char)_serial.read();
        if (resp.indexOf(expected) != -1) return true;
    }
    return false;
}

String GSM::sendATResponse(const char* cmd, uint16_t timeoutMs) {
    _serial.println(cmd);
    delay(timeoutMs);
    String resp = "";
    while (_serial.available()) {
        resp += (char)_serial.read();
    }
    return resp;
}

bool GSM::hasPaymentKeywords(const String& text) {
    return text.indexOf("credited")    != -1 ||
           text.indexOf("received")   != -1  ||
           text.indexOf("paid")       != -1  ||
           text.indexOf("Payment of") != -1  ||
           text.indexOf("Rs.")        != -1  ||
           text.indexOf("INR")        != -1;
}

bool GSM::hasRejectionKeywords(const String& text) {
    return text.indexOf("debited")    != -1 ||
           text.indexOf("failed")    != -1  ||
           text.indexOf("declined")  != -1  ||
           text.indexOf("reversed")  != -1  ||
           text.indexOf("deducted")  != -1;
}

float GSM::extractAmount(const String& text) {
    int idx = text.indexOf("Rs.");
    if (idx == -1) idx = text.indexOf("Rs ");
    if (idx == -1) idx = text.indexOf("INR ");

    if (idx == -1) return 0.0f;
    idx += 3;

    String numStr = "";
    for (int i = idx; i < (int)text.length(); i++) {
        char c = text[i];
        if (isDigit(c) || c == '.') numStr += c;
        else if (numStr.length() > 0) break;
    }

    return numStr.length() > 0 ? numStr.toFloat() : 0.0f;
}
