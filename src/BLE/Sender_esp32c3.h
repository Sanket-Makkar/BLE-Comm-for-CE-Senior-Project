#ifndef SENDER_ESP32C3_H
#define SENDER_ESP32C3_H

#include <NimBLEDevice.h>
#include <functional>

class Sender_esp32c3 {
public:
    using CommandCallback = std::function<void(const std::string&)>;

    Sender_esp32c3();
    void begin(const std::string& deviceName);
    void registerDataCallback(CommandCallback cb);
    void sendResponse(const std::string& response);
    void sendFloatsListAsString(const std::vector<float>& floatList);
    void startAdvertising();

private:
    NimBLEServer* pServer;
    NimBLECharacteristic* pCharacteristic;
    CommandCallback commandHandler;
};

#endif // SENDER_ESP32C3_H
