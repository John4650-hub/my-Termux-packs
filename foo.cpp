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
	std::cout<<"file opened success fully\n";
	return 0;
}
