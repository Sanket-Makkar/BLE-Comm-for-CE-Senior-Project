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

    String numberPart = incoming.substring(secondSpace + 1);
    numberPart.trim();
    int count = numberPart.toInt();

    if (count <= 0) {
        Serial.println("Invalid number or zero");
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

    Serial.println("✅ All chunks sent");
    }  
}