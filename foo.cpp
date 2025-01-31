#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <oboe/Oboe.h>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}


// Audio callback class to handle the streaming
class MyAudioCallback : public oboe::AudioStreamCallback {
public:
    MyAudioCallback(SwrContext *swrCtx, AVCodecContext *decCtx)
        : swrContext(swrCtx), decoderCtx(decCtx) {}

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override {
        uint8_t **converted_data = NULL;
        av_samples_alloc_array_and_samples(&converted_data, NULL, 2, numFrames, AV_SAMPLE_FMT_FLT, 0);

        int convert_ret = swr_convert(swrContext, converted_data, numFrames, (const uint8_t **)frame->data, numFrames);
        if (convert_ret < 0) {
            std::cerr << "Error during resampling\n";
            return oboe::DataCallbackResult::Stop;
        }

        memcpy(audioData, converted_data[0], numFrames * 2 * sizeof(float));
        av_freep(&converted_data[0]);
        return oboe::DataCallbackResult::Continue;
    }

    void setFrame(AVFrame *frame) {
        this->frame = frame;
    }

private:
    SwrContext *swrContext;
    AVCodecContext *decoderCtx;
    AVFrame *frame;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>\n";
        return -1;
    }

    AVFormatContext *formatCtx = NULL;
    int ret = avformat_open_input(&formatCtx, argv[1], NULL, NULL);
    if (ret < 0) {
        std::cerr << "Can't open file\n";
        return -1;
    }

    ret = avformat_find_stream_info(formatCtx, NULL);
    if (ret < 0) {
        std::cerr << "Could not find any info\n";
        return -1;
    }

    int stream_index = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (stream_index < 0) {
        std::cerr << "No audio stream found\n";
        return -1;
    }

    AVStream *media = formatCtx->streams[stream_index];
    AVCodec *decoder = avcodec_find_decoder(media->codecpar->codec_id);
    if (!decoder) {
        std::cerr << "Decoder not found\n";
        return -1;
    }

    AVCodecContext *decoder_ctx = avcodec_alloc_context3(decoder);
    if (!decoder_ctx) {
        std::cerr << "Could not allocate decoder context\n";
        return -1;
    }

    ret = avcodec_parameters_to_context(decoder_ctx, media->codecpar);
    if (ret < 0) {
        std::cerr << "Could not copy codec parameters\n";
        return -1;
    }

    ret = avcodec_open2(decoder_ctx, decoder, NULL);
    if (ret < 0) {
        std::cerr << "Failed to open decoder\n";
        return -1;
    }

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!packet || !frame) {
        std::cerr << "Could not allocate packet or frame\n";
        return -1;
    }

    SwrContext *swr_context = swr_alloc_set_opts(
        NULL,
        av_get_default_channel_layout(2),
        AV_SAMPLE_FMT_FLT,
        48000,
        av_get_default_channel_layout(decoder_ctx->channels),
        decoder_ctx->sample_fmt,
        decoder_ctx->sample_rate,
        0,
        NULL
    );
    if (!swr_context || swr_init(swr_context) < 0) {
        std::cerr << "Could not initialize resampler\n";
        return -1;
    }

    MyAudioCallback audioCallback(swr_context, decoder_ctx);

    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float)
           .setSampleRate(48000)
           .setChannelCount(oboe::ChannelCount::Stereo)
           .setCallback(&audioCallback);

    oboe::AudioStream *stream = nullptr;
    oboe::Result result = builder.openStream(&stream);
    if (result != oboe::Result::OK || stream == nullptr) {
        std::cerr << "Failed to open stream: " << oboe::convertToText(result) << std::endl;
        return -1;
    }

    stream->requestStart();

    while (av_read_frame(formatCtx, packet) >= 0) {
        if (packet->stream_index == stream_index) {
            ret = avcodec_send_packet(decoder_ctx, packet);
            if (ret < 0) {
                std::cerr << "Error sending packet for decoding\n";
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(decoder_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cerr << "Error during decoding\n";
                    return -1;
                }

                audioCallback.setFrame(frame);
            }
        }
        av_packet_unref(packet);
    }

    stream->stop();
    stream->close();
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&decoder_ctx);
    avformat_close_input(&formatCtx);
    swr_free(&swr_context);

    std::cout << "Decoding finished successfully\n";
    return 0;
}
