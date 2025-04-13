#include "Sender_esp32c3.h"

Sender_esp32c3::Sender_esp32c3() : pServer(nullptr), pService(nullptr), pCharacteristic(nullptr),
    commandHandler(nullptr), chimeHandler(nullptr) {}

void Sender_esp32c3::begin(const std::string& deviceName, bool enableNotify, bool enableIndicate, bool enableWrite, bool enableRead) {
    BLEDevice::init(String(deviceName.c_str()));
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new RocketBLEServerCallbacks(this));

    pService = pServer->createService(BLEUUID((uint16_t)0xFFE0));

    setupCharacteristics(enableNotify, enableIndicate, enableWrite, enableRead);

    pService->start();
    startAdvertising();
}

void Sender_esp32c3::setupCharacteristics(bool enableNotify, bool enableIndicate, bool enableWrite, bool enableRead) {
    uint32_t properties = 0;
    if (enableNotify) properties |= BLECharacteristic::PROPERTY_NOTIFY;
    if (enableIndicate) properties |= BLECharacteristic::PROPERTY_INDICATE;
    if (enableWrite)  properties |= BLECharacteristic::PROPERTY_WRITE;
    if (enableRead)   properties |= BLECharacteristic::PROPERTY_READ;

    pCharacteristic = pService->createCharacteristic(
        BLEUUID((uint16_t)0xFFE1),
        properties
    );

    if (enableNotify || enableIndicate) {
        pCharacteristic->addDescriptor(new BLE2902());
    }

    pCharacteristic->setCallbacks(new RocketBLECharacteristicCallbacks(this));
}

void Sender_esp32c3::startAdvertising() {
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();
}

void Sender_esp32c3::stopAdvertising() {
    BLEDevice::getAdvertising()->stop();
}

void Sender_esp32c3::registerDataCallback(CommandCallback cb) {
    commandHandler = cb;
}

void Sender_esp32c3::registerChimeCallback(FlagSetCallback cm) {
    chimeHandler = cm;
}

void Sender_esp32c3::sendResponse(const String& response, bool useIndicate) {
    if (pCharacteristic) {
        pCharacteristic->setValue(response.c_str());
        if (useIndicate) {
            pCharacteristic->indicate();
        } else {
            pCharacteristic->notify();
        }
    }
}

void Sender_esp32c3::sendIndication(const String& indication) {
    sendResponse(indication, true);
}

void Sender_esp32c3::broadcast(const String& message) {
    stopAdvertising();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    BLEAdvertisementData advData;
    advData.setName("Rocket_Battery_Broadcast");
    advData.setManufacturerData(message);
    pAdvertising->setAdvertisementData(advData);

    startAdvertising();
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

// Callbacks

void Sender_esp32c3::RocketBLECharacteristicCallbacks::onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    if (sender_->commandHandler) {
        sender_->commandHandler(std::string(value.c_str()));
    }
}

void Sender_esp32c3::RocketBLECharacteristicCallbacks::onRead(BLECharacteristic* pCharacteristic) {
    Serial.println("Log read request");
}

void Sender_esp32c3::RocketBLEServerCallbacks::onConnect(BLEServer* pServer) {
    if (sender_->chimeHandler) {
        sender_->chimeHandler(true);
    }
}

void Sender_esp32c3::RocketBLEServerCallbacks::onDisconnect(BLEServer* pServer) {
    if (sender_->chimeHandler) {
        sender_->chimeHandler(false);
    }
    if (sender_->pCharacteristic) {
        sender_->startAdvertising();
    }
}
