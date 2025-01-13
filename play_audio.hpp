#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <string>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    void play(const char* uri);
    void setStreamType(SLint32 streamType);
    std::string getError();

private:
    SLObjectItf mSlEngineObject{NULL};
    SLEngineItf mSlEngineInterface{NULL};
    SLObjectItf mSlOutputMixObject{NULL};
    SLObjectItf uriPlayerObject{NULL};
    SLPlayItf uriPlayerPlay{NULL};
    SLPrefetchStatusItf prefetchInterface{NULL};
    SLint32 androidStreamType{SL_ANDROID_STREAM_MEDIA};
    bool isPaused{false};
    std::string errorMessage;

    static void opensl_prefetch_callback(SLPrefetchStatusItf caller, void* pContext, SLuint32 event);
    static void opensl_player_callback(SLPlayItf caller, void* pContext, SLuint32 event);
};

#endif // AUDIO_PLAYER_H
