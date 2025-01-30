#include <iostream>
#include <string>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}

int main(int argc,char **argv){
	int ret{};
	AVFormatContext *formatCtx = NULL;
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
	const AVStream *stream = formatCtx->streams[stream_index];
	const AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
	if(!decoder){
		std::cout << "decoder not found\n";
		return -1;
	}
	std::cout<<"file opened success fully\n";
	return 0;
}
