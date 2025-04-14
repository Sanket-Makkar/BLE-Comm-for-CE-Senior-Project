#include "Chime.h"
#include "constants.h"

Chime::Chime(int piezoPin) : pin(piezoPin) {}

void Chime::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Chime::enqueueChime(ChimeTranslator::ChimeState state) {
    chimeQueue.push(state);
}

void Chime::playNextChime() {
    if (chimeQueue.empty()) return; // No chimes to play

    ChimeTranslator::ChimeState state = chimeQueue.front();
    chimeQueue.pop(); // Remove the chime from the queue

    switch (state) {
        case ChimeTranslator::CONNECT:
            playConnect();
            break;
        case ChimeTranslator::DISCONNECT:
            playDisconnect();
            break;
        case ChimeTranslator::SENT:
            playInfoSent();
            break;
        case ChimeTranslator::START:
            playStartup();
            break;
    }
}

void Chime::flushChimes() {
    while (!chimeQueue.empty()) {
        chimeQueue.pop(); // Clear the queue
    }
}

bool Chime::hasChimesToPlay() {
    return !chimeQueue.empty();
}

void Chime::toneSeq(const int* freqs, const int* durations, int count) {
    for (int i = 0; i < count; ++i) {
        if (freqs[i] > 0) {
            tone(pin, freqs[i], durations[i]);
        }
        delay(durations[i] + 30);  // Space between notes
    }
    noTone(pin);
}

// two tones
void Chime::playConnect() {
    int freqs[] = { 880, 1320 };
    int durations[] = { 120, 150 };
    toneSeq(freqs, durations, 2);
}

// descending tone
void Chime::playDisconnect() {
    int freqs[] = { 1320, 880 };
    int durations[] = { 150, 120 };
    toneSeq(freqs, durations, 2);
}

// beep
void Chime::playInfoSent() {
    int freqs[] = { 1000, 1200, 1000 };
    int durations[] = { 80, 80, 100 };
    toneSeq(freqs, durations, 3);
}

// ascending tone
void Chime::playStartup() {
    int freqs[] = { 660, 880, 1046 };
    int durations[] = { 100, 100, 200 };
    toneSeq(freqs, durations, 3);
}
