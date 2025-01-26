// Build flags: -lavcodec -lavformat -lavutil -lswresample

#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>", argv[0]);
        return -1;
    }

    int ret;

    // Step 1: Open audio file and open decoder
    AVFormatContext *format_context = NULL;
    ret = avformat_open_input(&format_context, argv[1], NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to open the media file '%s'\n", argv[1]);
        return -1;
    }

    ret = avformat_find_stream_info(format_context, NULL);
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to stream info \n");
        return -1;
    }

    printf("DEBUG: number of streams found: %d\n", format_context->nb_streams);
    int stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO,
                                           -1, -1, NULL, 0);
    if (stream_index < 0) {
        fprintf(stderr, "ERROR: no audio stream found in '%s' \n", argv[1]);
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

    printf(
        "DEBUG: Setting up decoder - sample format: %s, sample rate: %d Hz, "
        "channels: %d \n",
        av_get_sample_fmt_name(stream->codecpar->format),
        stream->codecpar->sample_rate, stream->codecpar->channels);

    // Step 2: Decode audio
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    SwrContext *swr_context = swr_alloc_set_opts(
        NULL, stream->codecpar->channel_layout, AV_SAMPLE_FMT_S16,
        stream->codecpar->sample_rate, stream->codecpar->channel_layout,
        stream->codecpar->format, stream->codecpar->sample_rate, 0, NULL);

    FILE *outfile = fopen(argv[2], "wb");

    while (av_read_frame(format_context, packet) == 0) {
        if (packet->stream_index != stream_index) continue;

        ret = avcodec_send_packet(decoder_ctx, packet);
        if (ret < 0 && (ret != AVERROR(EAGAIN))) {
            fprintf(stderr, "ERROR: failed to decode a frame\n");
        }
        while ((ret = avcodec_receive_frame(decoder_ctx, frame)) == 0) {
            AVFrame *resampled_frame = av_frame_alloc();
            resampled_frame->sample_rate = frame->sample_rate;
            resampled_frame->channel_layout = frame->channel_layout;
            resampled_frame->channels = frame->channels;
            resampled_frame->format = AV_SAMPLE_FMT_S16;

            ret = swr_convert_frame(swr_context, resampled_frame, frame);

            fwrite(resampled_frame->data[0], 1,
                   resampled_frame->nb_samples * resampled_frame->channels * 2,
                   outfile);

            av_frame_unref(frame);
            av_frame_free(&resampled_frame);
        }
    }
    
    swr_free(&swr_context);
    avcodec_close(decoder_ctx);
    avformat_close_input(&format_context);

    fclose(outfile);
}
