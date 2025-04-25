#include <Wire.h>
#include <SoftwareSerial.h>

//----------------------DEFINING VARIABLES--------------------//

#define INA232_ADDRESS 0x40 // 7-bit I2C address

#define CONFIGURATION_REGISTER_address 0x00
#define CONFIGURATION_REGISTER_value 0x5121 //0100011111111011
#define CALIBRATION_REGISTER_address 0x05
#define CALIBRATION_REGISTER_value 0x0026
#define SHUNT_VOLTAGE_REGISTER_address 0x01
#define BUS_VOLTAGE_REGISTER_address 0x02
#define POWER_REGISTER_address 0x03
#define CURRENT_REGISTER_address 0x04
#define ID_address 0x3E
// Declare variables
uint16_t SHUNT_VOLTAGE_REGISTER_value = 0x0000;
uint16_t BUS_VOLTAGE_REGISTER_value = 0x0000;
uint16_t POWER_REGISTER_value = 0x0000;
uint16_t CURRENT_REGISTER_value = 0x0000;
float val; 
// Function prototypes
void configureINA232();
uint16_t readRegister(uint8_t reg);
float readShuntVoltage();
float readBusVoltage();
float readCurrent();
float readPower();
float readID();
float reader();
String SHUNT_VOLTAGE_REGISTER_value_str();
String BUS_VOLTAGE_REGISTER_value_str();
String CURRENT_REGISTER_value_str();
String POWER_REGISTER_value_str();
void myTest();

bool testComplete;

SoftwareSerial mySerial(7, 8); // RX, TX

//----------------------DEFINING FUNCTIONS--------------------//

void setup() {
    testComplete = false;
    Wire.begin();
    Serial.begin(9600);
    mySerial.begin(9600);
    configureINA232();
    delay(1000);
}

void loop() {
    if (testComplete == false) {
        myTest();
        testComplete = true;
    }

    if (mySerial.available()) {
        String incoming = mySerial.readStringUntil('\n');
        incoming.trim();
        Serial.print("Received: ");
        Serial.println(incoming);

        int firstSpace = incoming.indexOf(' ');
        int secondSpace = incoming.indexOf(' ', firstSpace + 1);
        if (firstSpace == -1 || secondSpace == -1) {
            Serial.println("Invalid format");
            return;
        }

        String batteryNumStr = incoming.substring(0, firstSpace);
        int batteryNum = batteryNumStr.toInt();
        String measType = incoming.substring(firstSpace + 1, secondSpace);
        String numberPart = incoming.substring(secondSpace + 1);
        int count = numberPart.toInt();

        if (measType == "Voltage") {
            Serial.println("Measurement Type: Voltage");
            Serial.println("Voltage: " + String(readCurrent(), 2) + " V");
            Serial.println("Shunt Voltage: " + SHUNT_VOLTAGE_REGISTER_value_str() + " V");
            Serial.println("Bus Voltage: " + BUS_VOLTAGE_REGISTER_value_str() + " V");
        } else if (measType == "Current") {
            Serial.println("Measurement Type: Current");
            Serial.println("Current: " + CURRENT_REGISTER_value_str() + " A");
        } else if (measType == "Power") {
            Serial.println("Measurement Type: Power");
            Serial.println("Power: " + POWER_REGISTER_value_str() + " W");
        } else {
            Serial.println("Invalid measurement type");
            return;
        }

        String fullResponse = "";
        for (int i = 0; i < count; ++i) {
            //float val = random(10, 100) / 100.0;
            fullResponse += String(reader(measType), 2);
            if (i < count - 1) fullResponse += " ";
        }
    
        Serial.println("Sending response: " + fullResponse);

        const int chunkSize = 60;
        int start = 0;
        while (start < fullResponse.length()) {
            String chunk = fullResponse.substring(start, start + chunkSize);
            mySerial.println(chunk);
            delay(20);
            start += chunkSize;
        }

        Serial.println("All chunks sent");
    }
}

// Configure INA232
void configureINA232() {
    Serial.println("The 232 we are writing to for setup in hex: " + String(INA232_ADDRESS, HEX));

    // Write configuration register
    Wire.beginTransmission(INA232_ADDRESS);
    Wire.write(CONFIGURATION_REGISTER_address);
    Wire.write((CONFIGURATION_REGISTER_value >> 8) & 0xFF);
    Wire.write(CONFIGURATION_REGISTER_value & 0xFF);
    Wire.endTransmission();
    Serial.println("Config register written: 0x" + String(CONFIGURATION_REGISTER_value, HEX));

    // Write calibration register
    Wire.beginTransmission(INA232_ADDRESS);
    Wire.write(CALIBRATION_REGISTER_address);
    Wire.write((CALIBRATION_REGISTER_value >> 8) & 0xFF);
    Wire.write(CALIBRATION_REGISTER_value & 0xFF);
    Wire.endTransmission();
    Serial.println("Calib register written: 0x" + String(CALIBRATION_REGISTER_value, HEX));
}

// Read 16-bit register from INA232
uint16_t readRegister(uint8_t reg) {
    Wire.beginTransmission(INA232_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission(false); // Use repeated start
    Wire.requestFrom((uint8_t)INA232_ADDRESS, (uint8_t)2); // Explicit cast to uint8_t

    if (Wire.available() < 2) {
        Serial.println("Failed to read from register: 0x" + String(reg, HEX));
        return 0xFFFF; // Return an error value
    }

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    uint16_t val = (msb << 8) | lsb;
    Serial.print("Register 0x");
    Serial.print(reg, HEX);
    Serial.print(" read value: 0x");
    Serial.println(val, HEX);
    return val;
}


// Read shunt voltage in volts
float readShuntVoltage() {
    SHUNT_VOLTAGE_REGISTER_value = readRegister(SHUNT_VOLTAGE_REGISTER_address);
    SHUNT_VOLTAGE_REGISTER_value = SHUNT_VOLTAGE_REGISTER_value * 0.00013 * 0.74;
    if(SHUNT_VOLTAGE_REGISTER_value > 7){
     SHUNT_VOLTAGE_REGISTER_value = SHUNT_VOLTAGE_REGISTER_value - 3; 
    }
    return SHUNT_VOLTAGE_REGISTER_value;

}

// Read bus voltage in volts
float readBusVoltage() {
    BUS_VOLTAGE_REGISTER_value = readRegister(BUS_VOLTAGE_REGISTER_address);
    return BUS_VOLTAGE_REGISTER_value * 0.00125;
}

// Read current in amps
float readCurrent() {
    const float current_lsb = 0.000134277;
    CURRENT_REGISTER_value = readRegister(CURRENT_REGISTER_address);
    return CURRENT_REGISTER_value * current_lsb;
}

// Read power in watts
float readPower() {
    const float current_lsb = 0.000134277;
    POWER_REGISTER_value = readRegister(POWER_REGISTER_address);
    return POWER_REGISTER_value * current_lsb * 32;

}

float readID(){
    return readRegister(ID_address); 
}

// String converters
//String SHUNT_VOLTAGE_REGISTER_value_str() {
 //   return String(readShuntVoltage(), 2);
//}

String SHUNT_VOLTAGE_REGISTER_value_str() {
    const int numReadings = 10; // Number of readings to average
    float total = 0.0;

    // Take multiple readings and calculate the total
    for (int i = 0; i < numReadings; i++) {
        total += readShuntVoltage();
        delay(10); // Small delay between readings (optional)
    }

    // Calculate the average
    float average = total / numReadings;

    // Return the average as a string with 2 decimal places
    return String(average, 2);
}

String BUS_VOLTAGE_REGISTER_value_str() {
    return String(readBusVoltage(), 2);
}

String CURRENT_REGISTER_value_str() {
    return String(readCurrent(), 2);
}

String POWER_REGISTER_value_str() {
    return String(readPower(), 2);
}

String readID_str(){
    return String(((char)readID()), 2);
}
float reader(String meas){
    float return_val;
if (meas == "Voltage") {
            Serial.println("Measurement Type: Voltage");
            return_val = readShuntVoltage();
        } else if (meas == "Current") {
            return_val = readCurrent();
        } else if (meas == "Power") {
            return_val = readPower(); 
        } else {
            Serial.println("Invalid measurement type");
            return_val = 0;
        }
        return return_val; 
        }

// Debug/test output
void myTest() {
    Serial.println("Device ID: " + readID_str());
    Serial.println("Shunt voltage register value in float: " + SHUNT_VOLTAGE_REGISTER_value_str());
    Serial.println("Bus voltage register value in float: " + BUS_VOLTAGE_REGISTER_value_str());
    Serial.println("Current register value in float: " + CURRENT_REGISTER_value_str());
    Serial.println("Power register value in float: " + POWER_REGISTER_value_str());
}