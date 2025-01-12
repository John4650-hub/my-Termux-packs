#include "play_audio.hpp"
#include <assert.h>
#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

class MutexWithCondition {
public:
    MutexWithCondition() {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&condition, NULL);
        pthread_mutex_lock(&mutex);
    }
    ~MutexWithCondition() { pthread_mutex_unlock(&mutex); }
    void waitFor() { while (!occurred) pthread_cond_wait(&condition, &mutex); }
    void lockAndSignal() {
        pthread_mutex_lock(&mutex);
        occurred = true;
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
    }
private:
    volatile bool occurred{false};
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

AudioPlayer::AudioPlayer() {
    SLresult result = slCreateEngine(&mSlEngineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->Realize(mSlEngineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->GetInterface(mSlEngineObject, SL_IID_ENGINE, &mSlEngineInterface);
    assert(SL_RESULT_SUCCESS == result);

    SLuint32 const numWantedInterfaces = 0;
    result = (*mSlEngineInterface)->CreateOutputMix(mSlEngineInterface, &mSlOutputMixObject, numWantedInterfaces, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlOutputMixObject)->Realize(mSlOutputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
}

void AudioPlayer::play(const char* uri) {
    if (access(uri, R_OK) != 0) {
        errorMessage = "audio not found";
        return;
    }

    SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_uri, &format_mime};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mSlOutputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    SLuint32 const numWantedInterfaces = 2;
    SLInterfaceID wantedInterfaces[numWantedInterfaces]{ SL_IID_ANDROIDCONFIGURATION, SL_IID_PREFETCHSTATUS };
    SLboolean wantedInterfacesRequired[numWantedInterfaces]{ SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    SLresult result = (*mSlEngineInterface)->CreateAudioPlayer(mSlEngineInterface, &uriPlayerObject, &audioSrc, &audioSnk, numWantedInterfaces, wantedInterfaces, wantedInterfacesRequired);
    assert(SL_RESULT_SUCCESS == result);

    SLAndroidConfigurationItf androidConfig;
    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_ANDROIDCONFIGURATION, &androidConfig);
    assert(SL_RESULT_SUCCESS == result);

    result = (*androidConfig)->SetConfiguration(androidConfig, SL_ANDROID_KEY_STREAM_TYPE, &this->androidStreamType, sizeof(SLint32));
    assert(SL_RESULT_SUCCESS == result);

    result = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PREFETCHSTATUS, &prefetchInterface);
    assert(SL_RESULT_SUCCESS == result);

    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY, &uriPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    if (NULL == uriPlayerPlay) {
        errorMessage = "Cannot play audio";
        return;
    } else {
        result = (*uriPlayerPlay)->SetCallbackEventsMask(uriPlayerPlay, SL_PLAYEVENT_HEADSTALLED | SL_PLAYEVENT_HEADATEND);
        assert(SL_RESULT_SUCCESS == result);

        MutexWithCondition condition;
        result = (*uriPlayerPlay)->RegisterCallback(uriPlayerPlay, opensl_player_callback, &condition);
        assert(SL_RESULT_SUCCESS == result);

        result = (*prefetchInterface)->RegisterCallback(prefetchInterface, opensl_prefetch_callback, &condition);
        assert(SL_RESULT_SUCCESS == result);
        result = (*prefetchInterface)->SetCallbackEventsMask(prefetchInterface, SL_PREFETCHEVENT_FILLLEVELCHANGE | SL_PREFETCHEVENT_STATUSCHANGE);

        result = (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, SL_PLAYSTATE_PLAYING);
        assert(SL_RESULT_SUCCESS == result);

        condition.waitFor();
    }

    if (uriPlayerObject != NULL) (*uriPlayerObject)->Destroy(uriPlayerObject);
}

void AudioPlayer::setStreamType(SLint32 streamType) {
    this->androidStreamType = streamType;
}

SLint32 AudioPlayer::getCurrentStream() {
    return this->androidStreamType;
}

void AudioPlayer::pause() {
    if (uriPlayerPlay != NULL) {
        SLuint32 playState;
        (*uriPlayerPlay)->GetPlayState(uriPlayerPlay, &playState);
        if (playState == SL_PLAYSTATE_PLAYING) {
            (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, SL_PLAYSTATE_PAUSED);
            isPaused = true;
        } else if (playState == SL_PLAYSTATE_PAUSED) {
            (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, SL_PLAYSTATE_PLAYING);
            isPaused = false;
        }
    }
}

float AudioPlayer::getProgress() {
    if (uriPlayerPlay != NULL) {
        SLmillisecond duration, position;
        (*uriPlayerPlay)->GetDuration(uriPlayerPlay, &duration);
        (*uriPlayerPlay)->GetPosition(uriPlayerPlay, &position);
        return duration > 0 ? (float)position / duration : 0.0f;
    }
    return 0.0f;
}

void AudioPlayer::destroy() {
    if (uriPlayerPlay != NULL) {
        (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
    if (uriPlayerObject != NULL) {
        (*uriPlayerObject)->Destroy(uriPlayerObject);
        uriPlayerObject = NULL;
    }
}

std::string AudioPlayer::getError() {
    return errorMessage;
}

void AudioPlayer::opensl_prefetch_callback(SLPrefetchStatusItf caller, void* pContext, SLuint32 event) {
    if (event & SL_PREFETCHEVENT_STATUSCHANGE) {
        SLpermille level = 0;
        (*caller)->GetFillLevel(caller, &level);
        if (level == 0) {
            SLuint32 status;
            (*caller)->GetPrefetchStatus(caller, &status);
            if (status == SL_PREFETCHSTATUS_UNDERFLOW) {
                MutexWithCondition* cond = (MutexWithCondition*) pContext;
                cond->lockAndSignal();
            }
        }
    }
}

void AudioPlayer::opensl_player_callback(SLPlayItf caller, void* pContext, SLuint32 event) {
    MutexWithCondition* condition = (MutexWithCondition*) pContext;
    condition->lockAndSignal();
}

AudioPlayer::~AudioPlayer() {
    if (uriPlayerObject != NULL) { (*uriPlayerObject)->Destroy(uriPlayerObject); uriPlayerObject = NULL; }
    if (mSlOutputMixObject != NULL) { (*mSlOutputMixObject)->Destroy(mSlOutputMixObject); mSlOutputMixObject = NULL; }
    if (mSlEngineObject != NULL) { (*mSlEngineObject)->Destroy(mSlEngineObject); mSlEngineObject = NULL; }
}

int main(int argc, char** argv) {
    int c;
    char* streamType = NULL;
    while ((c = getopt(argc, argv, "s:")) != -1) {
        if (c == 's') {
            streamType = optarg;
        }
    }

    if (optind == argc) {
        return 1;
    }

    AudioPlayer player;

    if (streamType != NULL) {
        SLint32 streamTypeEnum;
        if (strcmp("alarm", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_ALARM;
        } else if (strcmp("media", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_MEDIA;
        } else if (strcmp("notification", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_NOTIFICATION;
        } else if (strcmp("ring", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_RING;
        } else if (strcmp("system", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_SYSTEM;
        } else if (strcmp("voice", streamType) == 0) {
            streamTypeEnum = SL_ANDROID_STREAM_VOICE;
        } else {
            player.getError() = "invalid streamtype";
            return 1;
        }
        player.setStreamType(streamTypeEnum);
    }

    for (int i = optind; i < argc; i++) {
        if (access(argv[i], R_OK) != 0) {
            player.getError() = "not a readable file";
            return 1;
        }
    }

    for (int i = optind; i < argc; i++) {
        player.play(argv[i]);
        if (!player.getError().empty()) {
            return 1;
        }
    }

    return 0;
}
