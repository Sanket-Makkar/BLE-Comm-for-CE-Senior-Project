#include "Orchestrator.h"
#include "Sender_esp32c3.h"
#include <Arduino.h>

using namespace std;

Orchestrator::Orchestrator(int uartRxPin, int uartTxPin, uint32_t baud)
    : uart(Serial1), rxPin(uartRxPin), txPin(uartTxPin), baudRate(baud) {
    // Register callbacks for BLE events
    bleSender.registerDataCallback([this](const std::string& command) {
        handleCommand(command);
    });
    bleSender.registerChimeCallback([this](bool state) {
        onChime(state);
    });
}

void Orchestrator::begin() {
    // Initialize Serial2 for UART communication with specified RX/TX pins
    uart.begin(baudRate, SERIAL_8N1, rxPin, txPin);
    bleSender.begin("ESP32-BLE-Sender", true, true, true, true); // Notify + Write, no Read
}

void Orchestrator::handleCommand(const string& command) {
    Serial.println("HandleCommand");
    // Format the incoming command
    String cmd = String(command.c_str());
    cmd.replace(';', ' ');

    // Extract MEASTYPE
    int firstSpace = cmd.indexOf(' ');
    int secondSpace = cmd.indexOf(' ', firstSpace + 1);
    String measurementType = cmd.substring(firstSpace + 1, secondSpace);
    Serial.println("Measurement Type: " + measurementType);

    // Send over UART (Serial2)
    uart.println(cmd);
    uart.flush();  // Ensure the command is sent before waiting for a response

    // Wait for multi-line UART responses from the other Arduino
    String response = waitForResponse();
    Serial.println("Response: " + response);

    // Format the response into a Python-style list
    String formattedResponse = formatResponsePyList(response, measurementType);
    Serial.println("Formatted Response: " + formattedResponse);

    // Split the formatted response into 19-byte chunks
    queueChunks(formattedResponse, 19);
}

String Orchestrator::waitForResponse() {
    String result;
    unsigned long start = millis();
    while (millis() - start < 3000) {  // 3 second timeout
        if (uart.available()) {
            String line = uart.readStringUntil('\n');
            line.trim();
            if (!line.isEmpty()) {
                if (!result.isEmpty()) {
                    result += ", ";
                }
                result += line;
            }
        }
    }
    Serial.println(result);
    return result;
}

String Orchestrator::formatResponsePyList(String response, String measurementType) {
    // Format the response into a list or other appropriate format
    String formattedResponse;
    if (measurementType == "Power" || measurementType == "Voltage" || measurementType == "Current") {
        formattedResponse = "[" + response + "]";
    } else {
        int firstSpace = response.indexOf(' ');
        formattedResponse = (firstSpace == -1) ? response : response.substring(0, firstSpace);
    }
    return formattedResponse;
}

void Orchestrator::queueChunks(String formattedResponse, int minChunkSize) {
    int start = 0;
    while (start < formattedResponse.length()) {
        int chunkSize = min(minChunkSize, (int)(formattedResponse.length() - start));
        String chunk = formattedResponse.substring(start, start + chunkSize);
        bleQueue.push(chunk);
        start += chunkSize;
    }
}

void Orchestrator::processBleQueue() {
    if (!bleQueue.empty() && millis() - lastSendTime >= bleSendInterval) {
        String nextChunk = bleQueue.front();
        bleQueue.pop();
        bleSender.sendIndication(nextChunk);
        lastSendTime = millis();
    }
}

// void Orchestrator::chunkAndSend(String formattedResponse, int minChunkSize) {
//     vector<String> chunks;
//     int start = 0;
//     while (start < formattedResponse.length()) {
//         int chunkSize = min(minChunkSize, (int)(formattedResponse.length() - start));
//         String chunk = formattedResponse.substring(start, start + chunkSize);
//         chunks.push_back(chunk);
//         start += chunkSize;
//     }

//     // Send each chunk via BLE indication
//     for (const auto& chunk : chunks) {
//         bleSender.sendIndication(chunk);
//     }
// }

// Chime handling
void Orchestrator::onChime(bool state) {
    Serial.println("🔔 Chime On: BLE connected");
}
