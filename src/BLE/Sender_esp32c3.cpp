#include "Sender_esp32c3.h"
#include "constants.h"

/* Quick callback adapter for bluetooth on xiao - helps us respond via a custom handler */
class BLECallbacks : public NimBLECharacteristicCallbacks {
public:
    BLECallbacks(Sender_esp32c3* parent) : _parent(parent) {}

    void onWrite(NimBLECharacteristic* pCharacteristic) override {
        std::string value = pCharacteristic->getValue();
        if (_parent->commandHandler) {
            _parent->commandHandler(value);
        }
    }

    // Kill advertising on connect - one at a time customer handling
    void onConnect(NimBLEServer* pServer) override {
        NimBLEDevice::getAdvertising()->stop();  // Stop advertising when connected
    }

    // Start advertising again when the customer leaves
    void onDisconnect(NimBLEServer* pServer) override {
        _parent->startAdvertising();  // Restart advertising after disconnection
    }

private:
    Sender_esp32c3* _parent;
};

/* The actual lower level orchestration of xiao BLE, 
 * defaults to everything is null but call begin to 
 * set things up.
 */
Sender_esp32c3::Sender_esp32c3()
    : pServer(nullptr), pCharacteristic(nullptr), commandHandler(nullptr) {}

/* Sets service uuid, characteristic uuid (both battery types)
 * starts the xiao as a BLE server and sets up advertising under
 * "deviceName"
 */
void Sender_esp32c3::begin(const std::string& deviceName) {
    NimBLEDevice::init(deviceName);
    pServer = NimBLEDevice::createServer();

    NimBLEService* pService = pServer->createService(BLEUUID(BLE_Characteristics::BATTERY_SERVICE_UUID));

    pCharacteristic = pService->createCharacteristic(
        BLEUUID(BLE_Characteristics::BATTERY_LEVEL_CHAR_UUID),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );

    pCharacteristic->setCallbacks(new BLECallbacks(this));
    pCharacteristic->setValue("Ready");

    pService->start();

    // Start advertising if it's not already started
    startAdvertising();
}

/* Set a callback to route data that comes in */
void Sender_esp32c3::registerDataCallback(CommandCallback cb) {
    commandHandler = cb;
}

/* basic response sender */
void Sender_esp32c3::sendResponse(const std::string& response) {
    if (pCharacteristic) {
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
    }
}

/* orchestrate a vector --> python list as string conversion 
 * and send it (low memory overhead this way) 
 */
void Sender_esp32c3::sendFloatsListAsString(const std::vector<float>& floatList) {
    // Convert the list of floats into a comma-separated string
    std::string result;
    for (size_t i = 0; i < floatList.size(); ++i) {
        result += std::to_string(floatList[i]);
        if (i < floatList.size() - 1) {
            result += ",";  // Add comma between floats
        }
    }

    Sender_esp32c3::sendResponse(result);
}

void Sender_esp32c3::startAdvertising() {
    delay(10);
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    if (pAdvertising->isAdvertising()) {
        return;
    }
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setInterval(BLE_Settings::ADVERTISING_INTERVAL); 
    pAdvertising->start();
}
