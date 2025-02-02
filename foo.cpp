#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <chrono>
#include "oboe/Oboe.h"
#include "oboe/FifoBuffer.h"
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}

void getPcmData(AVFormatContext *formatCtx, AVPacket *packet, AVCodecContext *decoder_ctx, AVFrame *frame, SwrContext *swr_context, int *stream_index,oboe::FifoBuffer &Buff) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	while (av_read_frame(formatCtx, packet) >= 0) {
					if (packet->stream_index == *stream_index) {
							 ret = avcodec_send_packet(decoder_ctx, packet);
							if (ret < 0) {
									std::cerr << "Error sending packet for decoding\n";
									break;
							}
							while (ret >= 0) {
									ret = avcodec_receive_frame(decoder_ctx, frame);
									if (ret == AVERROR(EAGAIN){
											std::cout<<"More packets are needed";
											break;
									} 
									else if(ret == AVERROR_EOF){
											std::cout<<"reached end of file";
											return;
									}
									else if (ret < 0) {
											std::cerr << "Error during decoding\n";
											break;
									}

									uint8_t **converted_data = NULL;
									av_samples_alloc_array_and_samples(
											&converted_data, NULL, 2, frame->nb_samples, AV_SAMPLE_FMT_FLT, 0
									);

									int convert_ret = swr_convert(
											swr_context, converted_data, frame->nb_samples,
											(const uint8_t **)frame->data, frame->nb_samples
									);

									if (convert_ret < 0) {
											std::cerr << "Error during resampling\n";
											break;
									}

								Buff.write(converted_data[0],frame->nb_samples);
								av_freep(&converted_data[0]);
							}
					}
					av_packet_unref(packet);
			}
}

uint32_t totalFrames(const char *filename) {
    int audio_stream_index = -1;
    uint32_t total_frames = 0;
		AVFormatContext *fmt_ctx_t = NULL;
		int ret = avformat_open_input(&fmt_ctx_t, filename, NULL, NULL);
		if (ret < 0) {
				std::cerr << "Can't open file\n";
				return 1;
		}
		if (avformat_find_stream_info(fmt_ctx_t, nullptr) < 0) {
        std::cerr << "Could not find stream information\n";
        avformat_close_input(&fmt_ctx_t);
        return 1;
    }
    for (unsigned int i = 0; i < fmt_ctx_t->nb_streams; i++) {
        if (fmt_ctx_t->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }

    if (audio_stream_index == -1) {
        std::cerr << "No audio stream found.\n";
        return 0;
    }

    AVPacket packet1;
    av_init_packet(&packet1);

    while (av_read_frame(fmt_ctx_t, &packet1) >= 0) {
        if (packet1.stream_index == audio_stream_index) {
            total_frames += packet1.duration;
        }
        av_packet_unref(&packet1);
    }
		avformat_close_input(&fmt_ctx_t);
    return total_frames;
}

class MyCallback : public oboe::AudioStreamCallback{
	public:
		MyCallback(oboe::FifoBuffer &buff) : mBuff(buff){}
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream *media,void *audioData, int32_t numFrames) override{
			auto floatData = static_cast<float*>(audioData);
			int32_t framesRead = mBuff.read(floatData,numFrames);
		return  oboe::DataCallbackResult::Continue;
		}
		void onErrorBeforeClose(oboe::AudioStream *media, oboe::Result error) override {
        std::cerr << "Error before close: " << oboe::convertToText(error) << std::endl;
    }

    void onErrorAfterClose(oboe::AudioStream *media, oboe::Result error) override {
        std::cerr << "Error after close: " << oboe::convertToText(error) << std::endl;
		}
	private:
		oboe::FifoBuffer &mBuff;
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

    FILE *outfile = fopen("output.pcm", "wb");
    if (!outfile) {
        std::cerr << "Could not open output file\n";
        avcodec_free_context(&decoder_ctx);
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
		//OBOE GOES HERE
		uint32_t bytesPerFrame = 8;
		uint32_t CapacityInFrames =totalFrames(argv[1]);
		oboe::FifoBuffer buff(bytesPerFrame,CapacityInFrames);
		std::thread t([&](){
				getPcmData(formatCtx, packet, decoder_ctx, frame, swr_context, &stream_index,buff);
				});
	t.detach();
		MyCallback audioCallback(buff);
		oboe::AudioStreamBuilder builder;
		builder.setCallback(&audioCallback);
		builder.setFormat(oboe::AudioFormat::Float);
		builder.setChannelCount(oboe::ChannelCount::Stereo);
		builder.setSampleRate(48000);

		oboe::AudioStream *mediaStream=nullptr;
		oboe::Result result = builder.openStream(&mediaStream);

		if(result != oboe::Result::OK){
			std::cerr<<"failed to create stream\n";
			return -1;
		}

		result=mediaStream->start();
		if(result != oboe::Result::OK){
			std::cerr << "failed to start stream\n";
			return -1;
		}
		int64_t duration = formatCtx->duration + (formatCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
    int duration_seconds = duration / (double)AV_TIME_BASE;
		std::cout<<"duration: "<<duration_seconds<<std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
		mediaStream->stop();
		mediaStream->close();

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&decoder_ctx);
    avformat_close_input(&formatCtx);
    swr_free(&swr_context);

    std::cout << "Decoding finished successfully\n";
    return 0;
}
