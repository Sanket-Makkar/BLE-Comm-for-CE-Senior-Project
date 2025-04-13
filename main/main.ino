#include "Sender_esp32c3.h"

Sender_esp32c3 sender;

void setup() {
    Serial.begin(115200);

    sender.begin("ESP32-BLE-Sender", true, true, false); // Notify + Write, no Read

    sender.registerDataCallback([](const std::string& data) {
        Serial.print("Received data: ");
        Serial.println(data.c_str());
        sender.sendIndication(String(("ACK: " + data).c_str()));
    });

    sender.registerChimeCallback([](bool connected) {
        Serial.println(connected ? "Device connected!" : "Device disconnected.");
    });
}

void loop() {
    delay(1000);
}
