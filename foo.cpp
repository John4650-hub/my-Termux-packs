#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>
#include "oboe/Oboe.h"
#include "oboe/FifoBuffer.h"
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}
std::atomic<bool> resume_decoding{false};
std::atomic<bool>* resume_decoding_ptr=&resume_decoding;


void format_time(int64_t secs){
	if (secs>1*AV_TIME_BASE){
	double total_seconds = static_cast<double>(secs) / 1000000.0; // 2500.721666 seconds
	int hours = static_cast<int>(total_seconds) / 3600; // 0 hours
	int minutes = (static_cast<int>(total_seconds) % 3600) / 60; // 41 minutes
	int seconds = static_cast<int>(total_seconds) % 60; // 40 seconds

	std::cout << "HH:MM:SS format: " << hours << ":" << minutes << ":" << seconds << std::endl;
}
}
void getPcmData(AVFormatContext *formatCtx, AVPacket *packet, AVCodecContext *decoder_ctx, AVFrame *frame, SwrContext *swr_context, int *stream_index,oboe::FifoBuffer &Buff,int64_t end_time) {
	int64_t current_pts = 0;
	bool end_time_scaled=false;
	while (av_read_frame(formatCtx, packet) >= 0) {
		if (packet->stream_index == *stream_index) {
				 int ret = avcodec_send_packet(decoder_ctx, packet);
				if (ret < 0){
						std::cerr << "Error sending packet for decoding\n";
						return;
				}
				while (ret >= 0) {
						ret = avcodec_receive_frame(decoder_ctx, frame);
						current_pts = frame->pts * av_q2d(formatCtx->streams[*stream_index]->time_base) * AV_TIME_BASE;
						
						if(!(end_time_scaled)){
							double diviser = static_cast<double>(current_pts)/static_cast<double>(end_time);
							end_time*=static_cast<int>(std::round(diviser));
							end_time_scaled=true;
						}
						if(current_pts>=end_time){
							//sleep
							while(!(resume_decoding.load())){
								std::this_thread::sleep_for(std::chrono::milliseconds(10));
							}
							end_time+=1*AV_TIME_BASE;
							resume_decoding_ptr->store(false);
						}
						if (ret == AVERROR(EAGAIN)){
								break;
						} 
						else if(ret == AVERROR_EOF){
								return;
						}
						else if (ret < 0) {
								std::cerr << "Error during decoding\n";
								return;
						}

						uint8_t **converted_data = NULL;
						av_samples_alloc_array_and_samples(
								&converted_data, NULL, 2, frame->nb_samples, AV_SAMPLE_FMT_S16, 0
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
		av_packet_unref(packet);
}
}
}

uint32_t totalFrames(const char *filename,int64_t end_time,AVFrame *frame) {
	int64_t current_pts = 0;
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
			current_pts = frame->pts * av_q2d(fmt_ctx_t->streams[audio_stream_index]->time_base) * AV_TIME_BASE;
			if(current_pts>=end_time){
				break;
			}
        if (packet1.stream_index == audio_stream_index) {
            total_frames += packet1.duration;
        }
        av_packet_unref(&packet1);
    }
		avformat_close_input(&fmt_ctx_t);
		std::cout<<"total_frames = "<<total_frames<<"\n";
    return total_frames+10000;
}

class MyCallback : public oboe::AudioStreamCallback{
	public:
		MyCallback(oboe::FifoBuffer &buff,uint8_t* data_storage ) : mBuff(buff),mdata_storage(data_storage){}
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream *media,void *audioData, int32_t numFrames) override{
			auto floatData = static_cast<float*>(audioData);
			int32_t framesRead = mBuff.read(floatData,numFrames);
			if (mBuff.getReadCounter()==mBuff.getWriteCounter()){
				mBuff.setReadCounter(0);
				mBuff.setWriteCounter(0);
				uint32_t capacity = mBuff.getBufferCapacityInFrames();
				delete[] mdata_storage;
				mdata_storage = nullptr;
				mdata_storage= new uint8_t[capacity];
				resume_decoding_ptr->store(true);
			}
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
		uint8_t* mdata_storage;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>\n";
        return -1;
    }
		int64_t start_time= 120* AV_TIME_BASE;
		int64_t end_time=140*AV_TIME_BASE;

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
		av_seek_frame(formatCtx,stream_index,start_time,AVSEEK_FLAG_BACKWARD);

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
        AV_SAMPLE_FMT_S16,
        decoder_ctx->sample_rate * 2,
        av_get_default_channel_layout(decoder_ctx->channels),
        decoder_ctx->sample_fmt,
        decoder_ctx->sample_rate * 2,
        0,
        NULL
    );
    if (!swr_context || swr_init(swr_context) < 0) {
        std::cerr << "Could not initialize resampler\n";
        return -1;
    }
		//OBOE GOES HERE
		std::atomic<uint64_t> read_index{}, write_index{};
		uint32_t CapacityInFrames =totalFrames(argv[1],end_time,frame);
		uint8_t* data_storage = new uint8_t[4 * CapacityInFrames];
		oboe::FifoBuffer buff(4,CapacityInFrames,&read_index,&write_index,data_storage);
		std::thread t([&](){
				getPcmData(formatCtx, packet, decoder_ctx, frame, swr_context, &stream_index,buff,end_time);
				});
	t.detach();
//wait writing to begin
while(true){
	if(buff.getWriteCounter()<1000){
		std::cout<<"seeking done\n";
		break;
	}
	std::cout<<"still seeking to right position\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
}
		MyCallback audioCallback(buff,data_storage);
		oboe::AudioStreamBuilder builder;
		builder.setCallback(&audioCallback);
		builder.setFormat(oboe::AudioFormat::I16);
		builder.setChannelCount(oboe::ChannelCount::Stereo);
		builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
		builder.setSampleRate(decoder_ctx->sample_rate * 2);

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
