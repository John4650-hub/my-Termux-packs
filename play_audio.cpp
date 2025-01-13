#include "play_audio.hpp"
#include <iostream>
#include <sndfile.h>

AudioPlayer::AudioPlayer() {
    Pa_Initialize();
}

AudioPlayer::~AudioPlayer() {
    Pa_Terminate();
}

void AudioPlayer::playAudio(const std::string& filePath) {
    SF_INFO sfInfo;
    SNDFILE* sndFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!sndFile) {
        std::cerr << "Error opening audio file: " << filePath << std::endl;
        return;
    }

    Pa_OpenDefaultStream(&stream, 0, sfInfo.channels, paFloat32, sfInfo.samplerate,
                         paFramesPerBufferUnspecified, paCallback, sndFile);
    Pa_StartStream(stream);

    // Wait until file is finished playing
    Pa_Sleep((sfInfo.frames / sfInfo.samplerate) * 1000);

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    sf_close(sndFile);
}

int AudioPlayer::paCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData) {
    SNDFILE* sndFile = static_cast<SNDFILE*>(userData);
    sf_count_t numFrames = sf_readf_float(sndFile, static_cast<float*>(outputBuffer), framesPerBuffer);
    if (numFrames < framesPerBuffer) {
        return paComplete;
    }
    return paContinue;
}
