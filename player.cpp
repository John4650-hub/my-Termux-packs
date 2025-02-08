#include "player.hpp"
#include "oboe/FifoBuffer.h"
#include "oboe/Oboe.h"
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}
std::atomic<bool> resume_decoding{false};
std::atomic<bool> *resume_decoding_ptr = &resume_decoding;
std::atomic<bool> completed{false};
std::atomic<bool> *completed_ptr=&completed;
std::atomic<int> seek_progress{0};
std::atomic<int> *seek_progress_ptr = &seek_progress;

//check whether audio is completed
void onCompletePlay(){
	while(!completed.load()){
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

// converts time from HH:MM:SS
int timeToSeconds(const std::string &seek_time) {
  int hour, minute, second;
  char delim;
  try {
    std::istringstream timeStream(seek_time);
    timeStream >> hour >> delim >> minute >> delim >> second;
    return (hour * 3600) + (minute * 60) + second;
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << "\n";
    std::cerr << "time must be valid in the format HH:MM:SS\n";
    std::exit(1);
  }
}

// converts time in seconds to HH:MM:SS
std::string formatSeconds(int total_seconds) {
  int hours = total_seconds / 3600;
  int minutes = (total_seconds % 3600) / 60;
  int seconds = total_seconds % 60;
  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
      << std::setfill('0') << minutes << ":" << std::setw(2)
      << std::setfill('0') << seconds;
  return oss.str();
}

// retrieves raw pcm data
void getPcmData(AVFormatContext *formatCtx, AVPacket *packet,
                AVCodecContext *decoder_ctx, AVFrame *frame,
                SwrContext *swr_context, int *stream_index,
                oboe::FifoBuffer &Buff, int64_t end_time) {
  int64_t current_pts = 0;
  bool end_time_scaled = false;
  while (av_read_frame(formatCtx, packet) >= 0) {
    if (packet->stream_index == *stream_index) {
      int ret = avcodec_send_packet(decoder_ctx, packet);
      if (ret < 0) {
        std::cerr << "Error sending packet for decoding\n";
        return;
      }
      while (ret >= 0) {
        ret = avcodec_receive_frame(decoder_ctx, frame);
        current_pts = frame->pts *
                      av_q2d(formatCtx->streams[*stream_index]->time_base) *
                      AV_TIME_BASE;
        seek_progress_ptr->store((current_pts / AV_TIME_BASE));
        if (!(end_time_scaled)) {
          double diviser =
              static_cast<double>(current_pts) / static_cast<double>(end_time);
          end_time *= static_cast<int>(std::round(diviser));
          end_time_scaled = true;
        }
        if (current_pts >= end_time) {
          // sleep
					
          while (!(resume_decoding.load())) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
          }
          end_time += 1 * AV_TIME_BASE;
          resume_decoding_ptr->store(false);
        }
        if (ret == AVERROR(EAGAIN)) {
          break;
        } else if (ret == AVERROR_EOF) {
          return;
        } else if (ret < 0) {
          std::cerr << "Error during decoding\n";
          return;
        }

        uint8_t **converted_data = NULL;
        av_samples_alloc_array_and_samples(
            &converted_data, NULL, 2, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);

        int convert_ret =
            swr_convert(swr_context, converted_data, frame->nb_samples,
                        (const uint8_t **)frame->data, frame->nb_samples);
        if (convert_ret < 0) {
          std::cerr << "Error during resampling\n";
          break;
        }
        // write pcm data to Fifobuffer
        Buff.write(converted_data[0], frame->nb_samples);
        av_freep(&converted_data[0]);
      }
      av_packet_unref(packet);
    }
  }
}

// callback class for creating oboe callback
class MyCallback : public oboe::AudioStreamCallback {
public:
  MyCallback(oboe::FifoBuffer &buff, uint8_t *data_storage, int duration_secs)
      : mBuff(buff), mdata_storage(data_storage),
        mDuration_secs(duration_secs) {}
  oboe::DataCallbackResult onAudioReady(oboe::AudioStream *media,
                                        void *audioData,
                                        int32_t numFrames) override {
    auto floatData = static_cast<float *>(audioData);
    int32_t framesRead = mBuff.read(floatData, numFrames);
		std::cout<<"Readercounter: "<<mBuff.getReadCounter()<<"\n";
		std::cout<< "writeCounter: "<<mBuff.getWriteCounter()<<"\n";
    if (mBuff.getReadCounter() == mBuff.getWriteCounter()) {
      mBuff.setReadCounter(0);
      mBuff.setWriteCounter(0);
      uint32_t capacity = mBuff.getBufferCapacityInFrames();
      delete[] mdata_storage;
      mdata_storage = nullptr;
      mdata_storage = new uint8_t[capacity];
      resume_decoding_ptr->store(true);
    }
    if (seek_progress.load() >= mDuration_secs){
			completed_ptr->store(true);
      return oboe::DataCallbackResult::Stop;
		}
    return oboe::DataCallbackResult::Continue;
  }
  void onErrorBeforeClose(oboe::AudioStream *media,
                          oboe::Result error) override {
    std::cerr << "Error before close: " << oboe::convertToText(error)
              << std::endl;
  }

  void onErrorAfterClose(oboe::AudioStream *media,
                         oboe::Result error) override {
    std::cerr << "Error after close: " << oboe::convertToText(error)
              << std::endl;
  }

private:
  oboe::FifoBuffer &mBuff;
  uint8_t *mdata_storage;
  int mDuration_secs;
};

/**
 * Take a the file name as input
 * and plays the audio file
 */
void play(const char *file_name, double rate, const std::string &seek_time) {
	completed_ptr->store(false);//reset the player
  if (rate < 0.1 || rate > 5.0) {
    std::cerr << "Rate must be from 0.1-3.0\n";
    return;
  }
  double sampleRate = rate;
	int seek_time_sec = timeToSeconds(seek_time);
  AVFormatContext *formatCtx = NULL;
  int ret = avformat_open_input(&formatCtx, file_name, NULL, NULL);
  if (ret < 0) {
    std::cerr << "Can't open file\n";
    return;
  }

  ret = avformat_find_stream_info(formatCtx, NULL);
  if (ret < 0) {
    std::cerr << "Could not find any info\n";
    return;
  }
int64_t duration_microseconds =
      formatCtx->duration +
      (formatCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
  int duration_seconds = duration_microseconds / (double)AV_TIME_BASE;

	if(seek_time_sec<duration_seconds){
		goto end;
	}
  int stream_index =
      av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  if (stream_index < 0) {
    std::cerr << "No audio stream found\n";
    return;
  }
  AVStream *media = formatCtx->streams[stream_index];
  int64_t start_time = av_rescale_q(seek_time_sec * AV_TIME_BASE,
                                    AV_TIME_BASE_Q, media->time_base);
  AVCodec *decoder = avcodec_find_decoder(media->codecpar->codec_id);
  if (!decoder) {
    std::cerr << "Decoder not found\n";
    return;
  }
  int64_t end_time = (1 * AV_TIME_BASE) + start_time;
  av_seek_frame(formatCtx, stream_index, start_time, AVSEEK_FLAG_ANY);

  AVCodecContext *decoder_ctx = avcodec_alloc_context3(decoder);

  if (!decoder_ctx) {
    std::cerr << "Could not allocate decoder context\n";
    return;
  }
  ret = avcodec_parameters_to_context(decoder_ctx, media->codecpar);
  if (ret < 0) {
    std::cerr << "Could not copy codec parameters\n";
    return;
  }

  ret = avcodec_open2(decoder_ctx, decoder, NULL);
  if (ret < 0) {
    std::cerr << "Failed to open decoder\n";
    return;
  }

  FILE *outfile = fopen("output.pcm", "wb");
  if (!outfile) {
    std::cerr << "Could not open output file\n";
    avcodec_free_context(&decoder_ctx);
    return;
  }

  AVPacket *packet = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  if (!packet || !frame) {
    std::cerr << "Could not allocate packet or frame\n";
    return;
  }
  SwrContext *swr_context = swr_alloc_set_opts(
      NULL, av_get_default_channel_layout(2), AV_SAMPLE_FMT_S16,
      decoder_ctx->sample_rate * sampleRate,
      av_get_default_channel_layout(decoder_ctx->channels),
      decoder_ctx->sample_fmt, decoder_ctx->sample_rate * sampleRate, 0, NULL);
  if (!swr_context || swr_init(swr_context) < 0) {
    std::cerr << "Could not initialize resampler\n";
    return;
  }
	std::atomic<uint64_t> read_index{}, write_index{};
  uint8_t *data_storage = new uint8_t[400000];
  oboe::FifoBuffer buff(4, 400000, &read_index, &write_index, data_storage);
  std::thread t([&]() {
    getPcmData(formatCtx, packet, decoder_ctx, frame, swr_context,
               &stream_index, buff, end_time);
  });

	t.detach();
  // wait for aome data to be written  to buffer before beginning playback
  while (true) {
    if (buff.getWriteCounter() < 1000) {
      std::cout << "seeking done\n";
      break;
    }
    std::cout << "still seeking to right position\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
	
  MyCallback audioCallback(buff, data_storage, duration_seconds);
  oboe::AudioStreamBuilder builder;
  builder.setCallback(&audioCallback);
  builder.setFormat(oboe::AudioFormat::I16);
  builder.setChannelCount(oboe::ChannelCount::Stereo);
  builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
  builder.setSampleRate(decoder_ctx->sample_rate * sampleRate);
  oboe::AudioStream *mediaStream = nullptr;
  oboe::Result result = builder.openStream(&mediaStream);

  if (result != oboe::Result::OK) {
    std::cerr << "failed to create stream\n";
    return;
  }

  result = mediaStream->start();
  if (result != oboe::Result::OK) {
    std::cerr << "failed to start stream\n";
    return;
  }
  std::cout << "duration: " << formatSeconds(duration_seconds) << std::endl;
  std::thread(onCompletePlay).join();
  mediaStream->stop();
  mediaStream->close();
  // free up all memory
  av_frame_free(&frame);
  av_packet_free(&packet);
  avcodec_free_context(&decoder_ctx);
  avformat_close_input(&formatCtx);
  swr_free(&swr_context);
end:
	return;
}
