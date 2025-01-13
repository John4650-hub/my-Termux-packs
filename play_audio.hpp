#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <string>

class AudioPlayer {
        public:
                AudioPlayer();
                ~AudioPlayer();
                void play(char const* uri);
                /**
                 * This allows setting the stream type (default:SL_ANDROID_STREAM_MEDIA):
                 * SL_ANDROID_STREAM_ALARM - same as android.media.AudioManager.STREAM_ALARM
                 * SL_ANDROID_STREAM_MEDIA - same as android.media.AudioManager.STREAM_MUSIC
                 * SL_ANDROID_STREAM_NOTIFICATION - same as android.media.AudioManager.STREAM_NOTIFICATION
                 * SL_ANDROID_STREAM_RING - same as android.media.AudioManager.STREAM_RING
                 * SL_ANDROID_STREAM_SYSTEM - same as android.media.AudioManager.STREAM_SYSTEM
                 * SL_ANDROID_STREAM_VOICE - same as android.media.AudioManager.STREAM_VOICE_CALL
                 */
                void setStreamType(SLint32 streamType) { this->androidStreamType = streamType; }
        private:
                SLObjectItf mSlEngineObject{NULL};
                SLEngineItf mSlEngineInterface{NULL};
                SLObjectItf mSlOutputMixObject{NULL};
                SLint32 androidStreamType{SL_ANDROID_STREAM_MEDIA};
}
#endif // AUDIO_PLAYER_H
