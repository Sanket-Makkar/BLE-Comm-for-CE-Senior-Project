// constants.h
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> 

#pragma once

namespace BLE_Characteristics {
    constexpr const char* BATTERY_SERVICE_UUID = "180F";    
    constexpr const char* BATTERY_LEVEL_CHAR_UUID = "2A19";  // Battery Level Characteristic UUID
}
namespace BLE_Settings
{
    constexpr int ADVERTISING_INTERVAL = 2500;
}
