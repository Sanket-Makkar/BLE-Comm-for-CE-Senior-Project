#pragma once

#include <Arduino.h>
#include "Sender_esp32c3.h"
#include <queue>
#include "Chime.h"

class Orchestrator {
public:
    Orchestrator(int uartRxPin, int uartTxPin, uint32_t baud = 9600, int piezoPin = 0);
    Chime chime;

    void begin();
    void handleCommand(const std::string& command);   // Called when BLE sends a command
    void onChime(int state);
    void processBleQueue(); // Call this regularly (from loop or task)

private:
    Sender_esp32c3 bleSender;
    HardwareSerial& uart;
    int rxPin, txPin;
    uint32_t baudRate;

    std::queue<String> bleQueue;
    unsigned long lastSendTime = 0;
    const unsigned long bleSendInterval = 80;  // ms between chunks

    String waitForResponse();
    String formatResponsePyList(String response, String measurementType);
    void queueChunks(String response, int chunkSize);
};
