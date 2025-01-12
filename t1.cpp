#include "audio_player.h"
#include <stdio.h>
#include <unistd.h>

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

