#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <argparse/argparse.hpp>
#include "player.hpp"

int main(int argc, char* argv[]) {
	[[maybe_unused]] std::string seek{};
	[[maybe_unused]] bool loop{};
	[[maybe_unused]] double rate{1.0};

	argparse::ArgumentParser program("oboe-play","1.0.0",argparse::default_arguments::help, false);
	
	program.add_argument("-i")
		.required()
		.help("Name of the audio file");
	
	program.add_argument("-s","--seek")
		. store_into(seek)
		.help("time to seek to before starting playback in HH:MM:SS format");
	
	program.add_argument("-r","--rate")
		.store_into(rate)
		.help("value to multiply the audio sample_rate with 0.0-3.0");

	program.add_argument("-l","--loop")
		.store_into(loop)
		.help("whether to repeat the audio playback on complete");

	try{
		program.parse_args(argc,argv);
		if (auto arg=program.present("-i")){
			const char* fname = arg->c_str();
			play(fname,rate,seek);
		}
	}catch(const std::runtime_error& err){
		std::cerr<<err.what()<<"\n";
		std::cerr<<program<<"\n";
		return 1;
	}
	return 0;
}
