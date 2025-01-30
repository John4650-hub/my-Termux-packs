#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}

int main(int argc,char **argv){
	int ret{},data_size{},i{},ch{};
	AVFormatContext *formatCtx = NULL;
	FILE *outfile;
	ret = avformat_open_input(&formatCtx,argv[1],NULL,NULL);
	if (ret<0){
		std::cout<<"Can't open file\n";
		return -1;
	}
	ret = avformat_find_stream_info(formatCtx,NULL);
	if (ret<0){
		std::cout<<"could not find any info\n";
		return -1;
	}
	int stream_index = av_find_best_stream(formatCtx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
	if(stream_index<0){
		std::cout<<"no audio stream found\n";
		return -1;
	}
	const AVStream *media = formatCtx->streams[stream_index];
	const AVCodec *decoder = avcodec_find_decoder(media->codecpar->codec_id);
	if(!decoder){
		std::cout << "decoder not found\n";
		return -1;
	}
	AVCodecContext *decoder_ctx = avcodec_alloc_context3(decoder);
outfile = fopen("output.pcm", "wb");
    if (!outfile) {
        av_free(decoder_ctx);
        return -1;
    }

	avcodec_parameters_to_context(decoder_ctx,media->codecpar);
	decoder_ctx->pkt_timebase = media->time_base;
	ret = avcodec_open2(decoder_ctx,decoder,NULL);
	if(ret<0){
		std::cout<<"failed to open decoder\n";
		return -1;
	}
	AVPacket *packet = av_packet_alloc();
	AVFrame *frame = av_frame_alloc();
	SwrContext *swr_context=swr_alloc_set_opts(
			NULL,
			av_get_default_channel_layout(2),
			AV_SAMPLE_FMT_S16,
			media->codecpar->sample_rate,
			av_get_default_channel_layout(2),
			(AVSampleFormat)media->codecpar->format,
			media->codecpar->sample_rate,
			0,
			NULL);
	swr_init(swr_context);
	std::cout<<"while started";
while (av_read_frame(formatCtx,packet)==0) {
        ret = avcodec_send_packet(decoder_ctx, packet);
				std::cout<<"ret: "<<ret<<", ";
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
					std::cerr<<"AVERROR OCCURED\n";
            return -1;
				}
        else if (ret < 0) {
					std::cerr<<"Error during decoding\n";
            return -1;
        }
        data_size = av_get_bytes_per_sample(decoder_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
					std::cerr<< "Failed to calculate data size\n";
            return -1;
        }
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < decoder_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
    }

	std::cout<<"file opened success fully\n";
	return 0;
}
