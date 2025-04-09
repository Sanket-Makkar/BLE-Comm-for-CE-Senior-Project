#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>  // Include the BLE2902 descriptor
#include "Sender_esp32c3.h"

// UUIDs for the service and characteristic
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-ba0987654321"

// Instantiate the Sender_esp32c3 class
Sender_esp32c3 sender;

void setup() {
    Serial.begin(115200);

    // Initialize and start the BLE device
    String deviceName = "ESP32-BLE-Sender";
    sender.begin(deviceName);

    // Register a custom callback to handle incoming commands
    sender.registerDataCallback([](const String& data) {
        Serial.println("Command received: " + data);
        if (data == "getList") {
            std::vector<String> myVector = {"item1", "item2", "item3"};
            String pythonList = Sender_esp32c3::vectorToPythonList(myVector);
            sender.sendResponse("Python List: " + pythonList);
        }
    });
}

void loop() {
    // Nothing to do here as the system works asynchronously
    delay(1000);
}
