#include "ERSpeaker.h"

#ifdef ER_SPEAKER_USE_HARDWARE
ERSpeaker::ERSpeaker(HardwareSerial& serial) : serial(&serial) {}
#else
ERSpeaker::ERSpeaker(SoftwareSerial& serial) : serial(&serial) {}
ERSpeaker::ERSpeaker(uint8_t rxPin, uint8_t txPin) {
    serial = new SoftwareSerial(rxPin, txPin);
    serial->begin(9600);
}
#endif

int ERSpeaker::begin() {
    if (!dfPlayer.begin(*serial)) {
        return -1; // Initialization failed
    }
    dfPlayer.volume(volume); // Set initial volume
    return 0; // Initialization successful
}

void ERSpeaker::setTrackList(TrackList trackList) {
    this->trackList = trackList;
}

void ERSpeaker::setTrackList(uint8_t count, uint32_t *durations) {
    this->trackList.count = count;
    this->trackList.durations = durations;
}

void ERSpeaker::playTrack(uint8_t track) {
    if (track < trackList.count) {
        dfPlayer.play(track + 1); // DFPlayer tracks are 1-indexed
        currentTrackIndex = track;
        timer = millis();
    }
}

void ERSpeaker::loopTrack(uint8_t track) {
    if (track < trackList.count) {
        dfPlayer.loop(track + 1); // DFPlayer tracks are 1-indexed
        currentTrackIndex = track;
        timer = millis();
    }
}

void ERSpeaker::queueTrack(uint8_t track) {
    if (track < trackList.count) {
        nextTrack = track;
    }
}

void ERSpeaker::skipToNextTrack() {
    if (nextTrack != -1) {
        playTrack(nextTrack);
        nextTrack = -1;
    } else {
        stop();
    }
}

void ERSpeaker::playNextTrack() {
    if (currentTrackIndex + 1 < trackList.count) {
        playTrack(currentTrackIndex + 1);
    }
}

void ERSpeaker::stop() {
    dfPlayer.stop();
    currentTrackIndex = -1;
    timer = 0;
    nextTrack = -1;
}

void ERSpeaker::setVolume(uint8_t volume) {
    if (volume > 30) volume = 30; // DFPlayer volume range is 0-30
    dfPlayer.volume(volume);
    this->volume = volume;
}

int ERSpeaker::getCurrentTrack() {
    return currentTrackIndex;
}

int ERSpeaker::getQueuedTrack() {
    return nextTrack;
}

int ERSpeaker::update() {
    timer += Time.delta_time();
    if (currentTrackIndex != -1) {
        uint32_t trackDuration = trackList.durations[currentTrackIndex];
        if (millis() - timer >= trackDuration) {
            int finishedTrack = currentTrackIndex;
            if (nextTrack != -1) {
                playTrack(nextTrack);
                nextTrack = -1;
            } else {
                stop();
            }
            return finishedTrack;
        }
    }
    return -1;
}
