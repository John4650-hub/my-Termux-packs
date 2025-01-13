#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include <string>
#include <portaudio.h>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    void playAudio(const std::string& filePath);

private:
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);
    PaStream* stream;
    // Additional private members for audio handling
};

#endif // AUDIOPLAYER_HPP
