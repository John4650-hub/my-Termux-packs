#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    void play(const char* uri);
    void setStreamType(SLint32 streamType) { this->androidStreamType = streamType; }
    SLmillisecond getPosition();
    SLmillisecond getDuration();

private:
    SLObjectItf mSlEngineObject{NULL};
    SLEngineItf mSlEngineInterface{NULL};
    SLObjectItf mSlOutputMixObject{NULL};
    SLObjectItf uriPlayerObject{NULL};
    SLPlayItf uriPlayerPlay{NULL};
    SLint32 androidStreamType{SL_ANDROID_STREAM_MEDIA};
};

class MutexWithCondition {
public:
    MutexWithCondition();
    ~MutexWithCondition();
    void waitFor();
    void lockAndSignal();
private:
    volatile bool occurred{false};
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

#endif // AUDIO_PLAYER_H
