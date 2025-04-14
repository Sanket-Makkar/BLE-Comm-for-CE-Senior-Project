#ifndef CHIME_H
#define CHIME_H

#include <Arduino.h>
#include <queue>
#include "constants.h" // For ChimeTranslator enum

class Chime {
public:
    Chime(int piezoPin);
    void begin();

    void enqueueChime(ChimeTranslator::ChimeState state);
    void playNextChime();
    bool hasChimesToPlay();
    void flushChimes(); 

private:
    int pin;
    std::queue<ChimeTranslator::ChimeState> chimeQueue;
    void toneSeq(const int* freqs, const int* durations, int count);

    void playConnect();
    void playDisconnect();
    void playInfoSent();
    void playStartup();
};

#endif
