#include "Sender_esp32c3.h"

Sender_esp32c3::Sender_esp32c3() : commandHandler(nullptr) {}

void Sender_esp32c3::begin(const String& deviceName) {
    // Initialize BLE device with the provided name
    BLEDevice::init(deviceName);

    // Create BLE server and service
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create characteristic with read, write, and notify properties
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Add the BLE2902 descriptor for notifications
    pCharacteristic->addDescriptor(new BLE2902());

    // Pass the current instance of Sender_esp32c3 to the callback
    pCharacteristic->setCallbacks(new MyCallbacks(this));

    // Set initial value for the characteristic
    pCharacteristic->setValue("Waiting for command...");

    // Start the service
    pService->start();

    // Setup advertising and start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Helps with iPhone connections
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();  // Start BLE advertising

    Serial.println("BLE Advertising started, waiting for connections...");
}

void Sender_esp32c3::registerDataCallback(CommandCallback cb) {
    commandHandler = cb;
}

void Sender_esp32c3::sendResponse(const String& response) {
    if (pCharacteristic) {
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();  // Notify the client of the response
    }
}

String Sender_esp32c3::vectorToPythonList(const std::vector<String>& vec) {
    String result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += "\"" + vec[i] + "\"";
        if (i < vec.size() - 1) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

void Sender_esp32c3::startAdvertising() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();  // Restart BLE advertising
}

void Sender_esp32c3::stopAdvertising() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->stop();  // Stop BLE advertising
}

// Definition for the BLECharacteristicCallbacks
void Sender_esp32c3::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    Serial.println("Received command: " + value);

    // If a custom callback is set, invoke it
    if (sender_->commandHandler) {
        sender_->commandHandler(value);
    }
}
