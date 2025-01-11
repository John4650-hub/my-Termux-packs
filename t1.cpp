#include "audio_player.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s <audio-file>\n", argv[0]);
        return 1;
    }

    AudioPlayer player;
    player.play(argv[1]);

    while (true) {
        SLmillisecond position = player.getPosition();
        SLmillisecond duration = player.getDuration();
        printf("Current position: %u ms, Duration: %u ms\n", position, duration);
        sleep(1); // Sleep for 1 second before checking the position again
    }

    return 0;
}
