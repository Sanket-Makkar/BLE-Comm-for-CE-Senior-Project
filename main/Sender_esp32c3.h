#ifndef SENDER_ESP32C3_H
#define SENDER_ESP32C3_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <functional>
#include <vector>

class Sender_esp32c3 {
public:
    using CommandCallback = std::function<void(const std::string&)>;
    using FlagSetCallback = std::function<void(bool)>;

    Sender_esp32c3();

    void begin(const std::string& deviceName, bool enableNotify, bool enableWrite, bool enableRead);
    void stopAdvertising();
    void sendResponse(const String& response, bool useIndicate);
    void sendIndication(const String& indication);
    void broadcast(const String& message);
    void registerDataCallback(CommandCallback cb);
    void registerChimeCallback(FlagSetCallback cm);
    String vectorToPythonList(const std::vector<String>& vec);

private:
    BLEServer* pServer;
    BLEService* pService;
    BLECharacteristic* pCharacteristic;

    CommandCallback commandHandler;
    FlagSetCallback chimeHandler;

    void setupCharacteristics(bool enableNotify, bool enableWrite, bool enableRead);
    void startAdvertising();

public:
    class RocketBLECharacteristicCallbacks : public BLECharacteristicCallbacks {
    public:
        RocketBLECharacteristicCallbacks(Sender_esp32c3* sender) : sender_(sender) {}
        void onWrite(BLECharacteristic* pCharacteristic) override;
        void onRead(BLECharacteristic* pCharacteristic) override;
    private:
        Sender_esp32c3* sender_;
    };

    class RocketBLEServerCallbacks : public BLEServerCallbacks {
    public:
        RocketBLEServerCallbacks(Sender_esp32c3* sender) : sender_(sender) {}
        void onConnect(BLEServer* pServer) override;
        void onDisconnect(BLEServer* pServer) override;
    private:
        Sender_esp32c3* sender_;
    };
};

#endif // SENDER_ESP32C3_H
