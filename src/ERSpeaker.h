#ifndef ER_SPEAKER_H
#define ER_SPEAKER_H

#include <Arduino.h>
#include <DFPlayerMini_Fast.h>

#include "ERTime.h"

#ifndef ER_SPEAKER_USE_HARDWARE
#include <SoftwareSerial.h>
#endif

typedef struct {
    uint8_t count;
    uint32_t *durations;
} TrackList;

class ERSpeaker {
public:
    #ifdef ER_SPEAKER_USE_HARDWARE
    ERSpeaker(HardwareSerial& serial);
    #else
    ERSpeaker(SoftwareSerial& serial);
    ERSpeaker(uint8_t rxPin, uint8_t txPin);
    #endif

    /** 
     * Initialize the speaker
     * @return 0 if initialization was successful, -1 if it failed
     */
    int begin();

    /** Set the list of tracks and their durations */
    void setTrackList(TrackList trackList);
    /** Set the list of tracks and their durations */
    void setTrackList(uint8_t count, uint32_t *durations);

    /** Play a specific track */
    void playTrack(uint8_t track);
    /** Loop a specific track */
    void loopTrack(uint8_t track);
    /** Queue a specific track to play after the current one finishes. Can only queue one track at a time */
    void queueTrack(uint8_t track);
    /** Play the queued track. stops playing if no track is queued */
    void skipToNextTrack();
    /** Play the next track in the list */
    void playNextTrack();

    /** Stop playback */
    void stop();
    /** Set the volume level */
    void setVolume(uint8_t volume);
    /** Get the index of the currently playing track or -1 if none is playing */
    int getCurrentTrack();
    /** Get the index of the queued track or -1 if none is queued */
    int getQueuedTrack();

    /** 
     * Update the speaker state. time.update() should be called before this function
     * @returns track index if a track finished playing, -1 otherwise.
     */
    int update();

private:
    DFPlayerMini_Fast dfPlayer;

    #ifdef ER_SPEAKER_USE_HARDWARE
    HardwareSerial* serial;
    #else
    SoftwareSerial* serial;
    #endif

    TrackList trackList = {0, nullptr};

    int currentTrackIndex = -1; // -1 indicates no track is currently playing
    uint32_t timer = 0;
    uint8_t volume = 20; // Default volume level (0-30)
    int nextTrack = -1; // -1 indicates no track is currently scheduled to play
};

#endif // ER_SPEAKER_H
