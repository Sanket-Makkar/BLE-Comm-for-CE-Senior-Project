#include <Arduino.h>
#include "Orchestrator.h"

// Define the pins used for UART communication on ESP32-C3
const int uartRxPin = D4;  // Example RX pin for ESP32-C3 (change as per your wiring)
const int uartTxPin = D3;  // Example TX pin for ESP32-C3 (change as per your wiring)
const int piezoPin = D7;
const uint32_t baudRate = 9600;

Orchestrator orchestrator(uartRxPin, uartTxPin, baudRate, piezoPin);

void setup() {
    // Start Serial Monitor for debugging
    Serial.begin(115200);  // This will be used for Serial Monitor output

    // Initialize the Orchestrator (handles UART and BLE)
    orchestrator.begin();

    // Additional setup or initialization if needed
    Serial.println("System Initialized. Ready to receive commands.");
}

void loop() {
    orchestrator.processBleQueue();
    orchestrator.chime.playNextChime();
}
