#include <Oboe.h>
#include <iostream>
#include <fstream>

class MyAudioCallback : public oboe::AudioStreamCallback {
public:
    MyAudioCallback(const std::string& filePath) : mFilePath(filePath) {
        mFile.open(mFilePath, std::ios::binary);
        if (!mFile.is_open()) {
            std::cerr << "Failed to open audio file: " << mFilePath << std::endl;
        }
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *stream, void *audioData, int32_t numFrames) override {
        if (!mFile.read(static_cast<char*>(audioData), numFrames * stream->getChannelCount() * sizeof(float))) {
            return oboe::DataCallbackResult::Stop;
        }
        return oboe::DataCallbackResult::Continue;
    }

    void onErrorBeforeClose(oboe::AudioStream *stream, oboe::Result error) override {
        std::cerr << "Error before close: " << oboe::convertToText(error) << std::endl;
    }

    void onErrorAfterClose(oboe::AudioStream *stream, oboe::Result error) override {
        std::cerr << "Error after close: " << oboe::convertToText(error) << std::endl;
    }

private:
    std::string mFilePath;
    std::ifstream mFile;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }

    std::string audioFilePath = argv[1];
    MyAudioCallback myCallback(audioFilePath);

    oboe::AudioStreamBuilder builder;
    builder.setCallback(&myCallback);
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setChannelCount(oboe::ChannelCount::Stereo);
    builder.setSampleRate(48000);

    oboe::AudioStream *stream = nullptr;
    oboe::Result result = builder.openStream(&stream);

    if (result != oboe::Result::OK) {
        std::cerr << "Failed to create stream. Error: " << oboe::convertToText(result) << std::endl;
        return 1;
    }

    result = stream->start();
    if (result != oboe::Result::OK) {
        std::cerr << "Failed to start stream. Error: " << oboe::convertToText(result) << std::endl;
        return 1;
    }

    std::cout << "Playing audio file: " << audioFilePath << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    stream->stop();
    stream->close();

    return 0;
}
