#include "player.hpp"
#include <argparse/argparse.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[]) {
  [[maybe_unused]] std::string seek{"00:00:00"};
  [[maybe_unused]] bool loop{false};
  [[maybe_unused]] double rate{1.0};

  argparse::ArgumentParser program("oboe-play", "1.0.0",
                                   argparse::default_arguments::help, false);

  program.add_argument("-i")
		.required()
		.nargs(argparse::nargs_pattern::at_least_one)
		.help("Name or comma separated names of the audio file(s)");

  program.add_argument("-s", "--seek")
      .store_into(seek)
      .help("time to seek to before starting playback in HH:MM:SS format");

  program.add_argument("-r", "--rate")
      .store_into(rate)
      .help("value to multiply the audio sample_rate with 0.0-3.0");

  program.add_argument("-l", "--loop")
      .store_into(loop)
      .help("whether to repeat the audio playback on complete");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << "\n";
    std::cerr << program << "\n";
    return 1;
  }
	auto files = program.get<std::vector<std::string>>("-i");
	if(loop){
		while(true){
			for(std::string str:files)
				play(str.c_str(), rate, seek);
		}
	}else
		for(std::string str:files)
			play(str.c_str(),rate,seek);
  return 0;
}
