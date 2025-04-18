#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8); // RX, TX

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    Serial.println("Ready to receive");
}

void loop() {
    if (mySerial.available()) {
    String incoming = mySerial.readStringUntil('\n');
    incoming.trim();
    Serial.print("Received: ");
    Serial.println(incoming);

    // Split the incoming message into parts
    int firstSpace = incoming.indexOf(' ');
    int secondSpace = incoming.indexOf(' ', firstSpace + 1);
    if (firstSpace == -1 || secondSpace == -1) {
        Serial.println("Invalid format");
        return;
    }

    // extract the battery num from batteryNum;measType;numberOfValues
    String batteryNumStr = incoming.substring(0, firstSpace);
    batteryNumStr.trim();
    int batteryNum = batteryNum.toInt();
    
    // extract the measType num from batteryNum;measType;numberOfValues
    String measType = incoming.substring(firstSpace + 1, secondSpace);
    measType.trim();

    // extract the numberPart num from batteryNum;measType;numberOfValues
    String numberPart = incoming.substring(secondSpace + 1);
    numberPart.trim();
    int count = numberPart.toInt();

    if (measType == "Voltage"){
        Serial.println("Measurement Type: Voltage");
    } 
    else if (measType == "Current") {
        Serial.println("Measurement Type: Current");
    } 
    else if (measType == "Power") {
        Serial.println("Measurement Type: Power");
    } 
    else if (measType == "Life") {
        Serial.println("Unknown measurement type");
        return;
    }
    else {
        Serial.println("Invalid measurement type");
        return;
    }

    // Build response with `count` random float values
    String fullResponse = "";
    for (int i = 0; i < count; ++i) {
      float val = random(10, 100) / 100.0; // 0.10 to 0.99
        fullResponse += String(val, 2);
        if (i < count - 1) fullResponse += " ";
    }

    Serial.print("Sending response: ");
    Serial.println(fullResponse);

    // Send in safe chunks
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