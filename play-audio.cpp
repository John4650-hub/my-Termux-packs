#include "audio_player.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

MutexWithCondition::MutexWithCondition() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    pthread_mutex_lock(&mutex);
}

MutexWithCondition::~MutexWithCondition() {
    pthread_mutex_unlock(&mutex);
}

void MutexWithCondition::waitFor() {
    while (!occurred) pthread_cond_wait(&condition, &mutex);
}

void MutexWithCondition::lockAndSignal() {
    pthread_mutex_lock(&mutex);
    occurred = true;
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);
}

AudioPlayer::AudioPlayer() {
    SLresult result = slCreateEngine(&mSlEngineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->Realize(mSlEngineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->GetInterface(mSlEngineObject, SL_IID_ENGINE, &mSlEngineInterface);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineInterface)->CreateOutputMix(mSlEngineInterface, &mSlOutputMixObject, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlOutputMixObject)->Realize(mSlOutputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
}

AudioPlayer::~AudioPlayer() {
    if (mSlOutputMixObject != NULL) { (*mSlOutputMixObject)->Destroy(mSlOutputMixObject); mSlOutputMixObject = NULL; }
    if (mSlEngineObject != NULL) { (*mSlEngineObject)->Destroy(mSlEngineObject); mSlEngineObject = NULL; }
    if (uriPlayerObject != NULL) { (*uriPlayerObject)->Destroy(uriPlayerObject); uriPlayerObject = NULL; }
}

void AudioPlayer::play(const char* uri) {
    SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_uri, &format_mime};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mSlOutputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    SLuint32 const numWantedInterfaces = 2;
    SLInterfaceID wantedInterfaces[numWantedInterfaces]{ SL_IID_ANDROIDCONFIGURATION, SL_IID_PREFETCHSTATUS };
    SLboolean wantedInterfacesRequired[numWantedInterfaces]{ SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    SLresult result = (*mSlEngineInterface)->CreateAudioPlayer(mSlEngineInterface, &uriPlayerObject, &audioSrc, &audioSnk,
            numWantedInterfaces, wantedInterfaces, wantedInterfacesRequired);
    assert(SL_RESULT_SUCCESS == result);

    SLAndroidConfigurationItf androidConfig;
    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_ANDROIDCONFIGURATION, &androidConfig);
    assert(SL_RESULT_SUCCESS == result);

    result = (*androidConfig)->SetConfiguration(androidConfig, SL_ANDROID_KEY_STREAM_TYPE, &this->androidStreamType, sizeof(SLint32));
    assert(SL_RESULT_SUCCESS == result);

    result = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    SLPrefetchStatusItf prefetchInterface;
    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PREFETCHSTATUS, &prefetchInterface);
    assert(SL_RESULT_SUCCESS == result);

    result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY, &uriPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    if (uriPlayerPlay == NULL) {
        fprintf(stderr, "Cannot play '%s'\n", uri);
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
}

SLmillisecond AudioPlayer::getPosition() {
    SLmillisecond position;
    SLresult result = (*uriPlayerPlay)->GetPosition(uriPlayerPlay, &position);
    assert(SL_RESULT_SUCCESS == result);
    return position;
}

SLmillisecond AudioPlayer::getDuration() {
    SLmillisecond duration = SL_TIME_UNKNOWN;
    SLresult result = (*uriPlayerPlay)->GetDuration(uriPlayerPlay, &duration);
    assert(SL_RESULT_SUCCESS == result);
    return duration;
}

void opensl_prefetch_callback(SLPrefetchStatusItf caller, void* pContext, SLuint32 event) {
    if (event & SL_PREFETCHEVENT_STATUSCHANGE) {
        SLpermille level = 0;
        (*caller)->GetFillLevel(caller, &level);
        if (level == 0) {
            SLuint32 status;
            (*caller)->GetPrefetchStatus(caller, &status);
            if (status == SL_PREFETCHSTATUS_UNDERFLOW) {
                printf("play-audio: underflow when prefetching data\n");
                MutexWithCondition* cond = (MutexWithCondition*) pContext;
                cond->lockAndSignal();
            }
        }
    }
}

void opensl_player_callback(SLPlayItf /*caller*/, void* pContext, SLuint32 /*event*/) {
    MutexWithCondition* condition = (MutexWithCondition*) pContext;
    condition->lockAndSignal();
}
