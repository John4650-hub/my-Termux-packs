#include "oboe/Oboe.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include "decoder.hpp"
#include <thread>
#include <chrono>

class AudioPlayer : public oboe::AudioStreamCallback {
public:
    AudioPlayer(const uint8_t* pcmData, size_t dataSize)
        : mPcmData(pcmData), mDataSize(dataSize), mReadIndex(0) {}

    void start() {
        oboe::AudioStreamBuilder builder;
        builder.setFormat(oboe::AudioFormat::I16)
               .setChannelCount(oboe::ChannelCount::Stereo)
               .setSampleRate(48000)
               .setCallback(this);

        oboe::Result result = builder.openStream(&mStream);
        if (result != oboe::Result::OK) {
            // Handle error
            return;
        }

        mStream->requestStart();
    }

    void stop() {
        if (mStream) {
            mStream->requestStop();
            mStream->close();
            mStream = nullptr;
        }
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream* stream, void* audioData, int32_t numFrames) override {
        int32_t framesToCopy = std::min(numFrames, static_cast<int32_t>((mDataSize - mReadIndex) / 2));

        std::memcpy(audioData, mPcmData + mReadIndex, framesToCopy * 2);
        mReadIndex += framesToCopy * 2;

        if (framesToCopy < numFrames) {
            // If we reach the end of the PCM data, stop the stream
            return oboe::DataCallbackResult::Stop;
        }

        return oboe::DataCallbackResult::Continue;
    }

private:
    const uint8_t* mPcmData;
    size_t mDataSize;
    int32_t mReadIndex;
    oboe::AudioStream* mStream = nullptr;
};

void playAudio(const uint8_t* pcmData, size_t dataSize) {
    AudioPlayer player(pcmData, dataSize);
    player.start();

    // Wait for the audio to finish playing
    std::this_thread::sleep_for(std::chrono::seconds(dataSize / 48000 / 2));

    player.stop();
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <input file> <start time ms>\n", argv[0]);
		return -1;
	}

	const char *input_filename = argv[1];
	int64_t start_time_ms = atoll(argv[2]);
	int64_t pcm_data_size;

	uint8_t *pcm_data = decode_mp3_to_pcm(input_filename, start_time_ms, &pcm_data_size);
	playAudio(pcm_data,pcm_data_size);
	return 0;
}
