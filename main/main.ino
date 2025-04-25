#include <Arduino.h>
#include "Orchestrator.h"

// Define the pins used for UART communication on ESP32-C3
const int uartRxPin = D4;  // Example RX pin for ESP32-C3 (change as per your wiring)
const int uartTxPin = D3;  // Example TX pin for ESP32-C3 (change as per your wiring)
const int piezoPin = D7;
const uint32_t baudRate = 9600;

Orchestrator orchestrator(uartRxPin, uartTxPin, baudRate, piezoPin);

void setup() {
    // Run orchestrator BLE queue processing in its own task
    xTaskCreate(
        [](void* parameter) {
            for (;;) {
                orchestrator.processBleQueue();
                vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to yield to other tasks
            }
        },
        "ProcessBleQueueTask",
        2048, // Stack size
        NULL,
        1,    // Priority
        NULL
    );

    // Run orchestrator chime processing in its own task
    xTaskCreate(
        [](void* parameter) {
            for (;;) {
                orchestrator.chime.playNextChime();
                vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to yield to other tasks
            }
        },
        "PlayNextChimeTask",
        2048, // Stack size
        NULL,
        1,    // Priority
        NULL
    );

    // Start Serial Monitor for debugging
    Serial.begin(115200);  // This will be used for Serial Monitor output

    // Initialize the Orchestrator (handles UART and BLE)
    orchestrator.begin();

    // Additional setup or initialization if needed
    Serial.println("System Initialized. Ready to receive commands.");
}

void loop() {
    delay(1);
}
