#include "decoder.hpp"
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>

uint8_t* decode_mp3_to_pcm(const char *input_filename, int64_t start_time_ms, int64_t *pcm_data_size) {
    AVFormatContext *formatContext = NULL;
    AVCodecContext *codecContext = NULL;
    AVCodec *codec = NULL;
    AVPacket packet;
    AVFrame *frame = NULL;
    SwrContext *swrContext = NULL;
    uint8_t *pcm_data = NULL;
    int pcm_data_capacity = 0;
    *pcm_data_size = 0;

    // Initialize libavformat and register all codecs
    av_register_all();

    // Open the input file
    if (avformat_open_input(&formatContext, input_filename, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open input file.\n");
        return NULL;
    }

    // Find the best stream
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        fprintf(stderr, "Could not find stream information.\n");
        return NULL;
    }

    // Find the audio stream index
    int audioStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    if (audioStreamIndex == -1) {
        fprintf(stderr, "Could not find audio stream.\n");
        return NULL;
    }

    // Get the codec parameters and find the decoder
    AVCodecParameters *codecParameters = formatContext->streams[audioStreamIndex]->codecpar;
    codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        fprintf(stderr, "Could not find codec.\n");
        return NULL;
    }

    // Initialize the codec context
    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParameters);
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec.\n");
        return NULL;
    }

    // Set up the resampler
    swrContext = swr_alloc_set_opts(NULL,
                                    AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_FLT, codecContext->sample_rate,
                                    av_get_default_channel_layout(codecContext->channels), codecContext->sample_fmt, codecContext->sample_rate,
                                    0, NULL);
    swr_init(swrContext);

    // Allocate frame and packet
    frame = av_frame_alloc();
    av_init_packet(&packet);

    // Seek to the start time
    int64_t seek_target = av_rescale_q(start_time_ms, AV_TIME_BASE_Q, formatContext->streams[audioStreamIndex]->time_base);
    av_seek_frame(formatContext, audioStreamIndex, seek_target, AVSEEK_FLAG_BACKWARD);

    // Decode the frames
    int64_t end_time = start_time_ms + 60000; // 60 seconds
    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == audioStreamIndex) {
            avcodec_send_packet(codecContext, &packet);
            while (avcodec_receive_frame(codecContext, frame) >= 0) {
                // Resample the audio
                uint8_t *outputBuffer;
                int outputBufferSize = av_samples_alloc(&outputBuffer, NULL, 2, frame->nb_samples, AV_SAMPLE_FMT_FLT, 0);
                int samplesConverted = swr_convert(swrContext, &outputBuffer, outputBufferSize, (const uint8_t **)frame->data, frame->nb_samples);

                // Reallocate PCM data buffer if necessary
                int new_size = *pcm_data_size + samplesConverted * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
                if (new_size > pcm_data_capacity) {
                    pcm_data_capacity = new_size * 2;
                    pcm_data = realloc(pcm_data, pcm_data_capacity);
                }

                // Copy the PCM data to the buffer
                memcpy(pcm_data + *pcm_data_size, outputBuffer, samplesConverted * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT));
                *pcm_data_size += samplesConverted * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
                av_free(outputBuffer);

                // Check if we have reached the end time
                int64_t current_time = av_rescale_q(frame->pts, formatContext->streams[audioStreamIndex]->time_base, AV_TIME_BASE_Q);
                if (current_time >= end_time) {
                    break;
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Clean up
    swr_free(&swrContext);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    return pcm_data;
}

