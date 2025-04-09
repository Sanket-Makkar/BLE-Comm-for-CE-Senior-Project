#ifndef SENDER_ESP32C3_H
#define SENDER_ESP32C3_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>
#include <vector>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-ba0987654321"

class Sender_esp32c3 {
public:
    using CommandCallback = std::function<void(const String&)>;
    
    Sender_esp32c3();
    
    void begin(const String& deviceName);
    void registerDataCallback(CommandCallback cb);
    void sendResponse(const String& response);
    static String vectorToPythonList(const std::vector<String>& vec);
    void startAdvertising();
    void stopAdvertising();

private:
    BLECharacteristic *pCharacteristic;
    CommandCallback commandHandler;

    // BLECharacteristicCallback class to handle write events
    class MyCallbacks : public BLECharacteristicCallbacks {
    public:
        MyCallbacks(Sender_esp32c3* sender) : sender_(sender) {}
        void onWrite(BLECharacteristic *pCharacteristic) override;

    private:
        Sender_esp32c3* sender_;
    };
};

#endif // SENDER_ESP32C3_H
