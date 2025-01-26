#include <stdio.h>
#include <oboe/Oboe.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>

class AudioEngine : public oboe::AudioStreamCallback {
public:
    bool start();
    void stop();

    // Oboe callback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    void enqueuePCMData(uint8_t *data, size_t size);

private:
    std::shared_ptr<oboe::AudioStream> mStream;
    std::vector<uint8_t> mBuffer;
    std::mutex mMutex;
    std::condition_variable mCondVar;
};

bool AudioEngine::start() {
    oboe::AudioStreamBuilder* builder=new oboe::AudioStreamBuilder();
    builder->setDirection(oboe::Direction::Output)
			->setFormat(oboe::AudioFormat::I16)
			->setChannelCount(oboe::ChannelCount::Stereo)
    ->.setSampleRate(44100)
		->.setCallback(this);

    oboe::Result result = builder->openStream(mStream);
    if (result != oboe::Result::OK) return false;

    return mStream->start() == oboe::Result::OK;
}

void AudioEngine::stop() {
    if (mStream) {
        mStream->stop();
        mStream->close();
    }
}

oboe::DataCallbackResult AudioEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    std::unique_lock<std::mutex> lock(mMutex);
    mCondVar.wait(lock, [this]() { return !mBuffer.empty(); });

    size_t bytesToCopy = numFrames * mStream->getChannelCount() * sizeof(int16_t);
    if (bytesToCopy > mBuffer.size()) bytesToCopy = mBuffer.size();

    memcpy(audioData, mBuffer.data(), bytesToCopy);
    mBuffer.erase(mBuffer.begin(), mBuffer.begin() + bytesToCopy);

    return oboe::DataCallbackResult::Continue;
}

void AudioEngine::enqueuePCMData(uint8_t *data, size_t size) {
    std::lock_guard<std::mutex> lock(mMutex);
    mBuffer.insert(mBuffer.end(), data, data + size);
    mCondVar.notify_one();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return -1;
    }

    int ret;
    AVFormatContext *format_context = NULL;
    ret = avformat_open_input(&format_context, argv[1], NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to open the media file '%s'\n", argv[1]);
        return -1;
    }

    ret = avformat_find_stream_info(format_context, NULL);
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to find stream info\n");
        return -1;
    }

    int stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (stream_index < 0) {
        fprintf(stderr, "ERROR: no audio stream found in '%s'\n", argv[1]);
        return -1;
    }

    const AVStream *stream = format_context->streams[stream_index];
    const AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        fprintf(stderr, "ERROR: no decoder found\n");
        return -1;
    }

    AVCodecContext *decoder_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(decoder_ctx, stream->codecpar);
    ret = avcodec_open2(decoder_ctx, decoder, NULL);
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to open the decoder\n");
        return -1;
    }

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    SwrContext *swr_context = swr_alloc_set_opts(
        NULL, av_get_default_channel_layout(2), AV_SAMPLE_FMT_S16,
        stream->codecpar->sample_rate, av_get_default_channel_layout(2),
        (AVSampleFormat)stream->codecpar->format, stream->codecpar->sample_rate, 0, NULL);
    swr_init(swr_context);

    AudioEngine audioEngine;
    if (!audioEngine.start()) {
        fprintf(stderr, "ERROR: failed to start audio engine\n");
        return -1;
    }

    std::thread decodeThread([&]() {
        while (av_read_frame(format_context, packet) == 0) {
            if (packet->stream_index != stream_index) continue;

            ret = avcodec_send_packet(decoder_ctx, packet);
            if (ret < 0 && (ret != AVERROR(EAGAIN))) {
                fprintf(stderr, "ERROR: failed to decode a frame\n");
                continue;
            }

            while ((ret = avcodec_receive_frame(decoder_ctx, frame)) == 0) {
                uint8_t *output[2];
                int out_samples = swr_convert(swr_context, output, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
                size_t out_size = out_samples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) * 2; // 2 channels
                audioEngine.enqueuePCMData(output[0], out_size);
            }
        }
    });

    decodeThread.join();
    audioEngine.stop();

    av_packet_free(&packet);
    av_frame_free(&frame);
    swr_free(&swr_context);
    avcodec_close(decoder_ctx);
    avformat_close_input(&format_context);

    return 0;
}
