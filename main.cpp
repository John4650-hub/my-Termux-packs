#include "oboe/Oboe.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

class MyAudioCallback : public oboe::AudioStreamCallback {
public:
    MyAudioCallback(const std::string& filePath) : mFilePath(filePath) {
        mFile.open(mFilePath, std::ios::binary);
        if (!mFile.is_open()) {
            std::cerr << "Failed to open audio file: " << mFilePath << std::endl;
        }
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *media, void *audioData, int32_t numFrames) override {
        if (!mFile.read(static_cast<char*>(audioData), numFrames * media->getChannelCount() * sizeof(float))) {
            return oboe::DataCallbackResult::Stop;
        }
        return oboe::DataCallbackResult::Continue;
    }

    void onErrorBeforeClose(oboe::AudioStream *media, oboe::Result error) override {
        std::cerr << "Error before close: " << oboe::convertToText(error) << std::endl;
    }

    void onErrorAfterClose(oboe::AudioStream *media, oboe::Result error) override {
        std::cerr << "Error after close: " << oboe::convertToText(error) << std::endl;
    }

private:
    std::string mFilePath;
    std::ifstream mFile;
};

void playAudio(const std::string& filePath,int ptime) {
    MyAudioCallback myCallback(filePath);

    oboe::AudioStreamBuilder builder;
    builder.setCallback(&myCallback);
    builder.setFormat(oboe::AudioFormat::I16);
	  builder.setChannelCount(oboe::ChannelCount::Μono);
    //builder.setSampleRate(48000);

    oboe::AudioStream *media = nullptr;
    oboe::Result result = builder.openStream(&media);

    if (result != oboe::Result::OK) {
        std::cerr << "Failed to create stream. Error: " << oboe::convertToText(result) << std::endl;
        return;
    }

    result = media->start();
    if (result != oboe::Result::OK) {
        std::cerr << "Failed to start stream. Error: " << oboe::convertToText(result) << std::endl;
        media->close();
        return;
    }

    // Sleep for the duration of the audio file
    std::this_thread::sleep_for(std::chrono::seconds(ptime*60)); // Adjust the duration as needed

    media->stop();
    media->close();
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }

    std::string audioFilePath = argv[1];
		std::string arg2 = argv[2];
    int ptime = std::stoi(arg2);
    playAudio(audioFilePath,ptime);

    return 0;
}
