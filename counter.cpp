#include <iostream>
#include <chrono>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

std::atomic<bool> counting{true};
extern std::atomic<int> stateTime;
extern std::string timeCount;
extern ftxui::ScreenInteractive screen;

void startTimer(){
	std::thread([&](){
	while(stateTime>=0 && counting == true){
		screen.PostEvent(ftxui::Event::Custom);
		stateTime-=1;
		timeCount = std::to_string(stateTime);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		}
}
).detach();
}

// function to start timer


//function to pause the timer
void pauseTimer(){
	if(counting)
		counting = false;
	else{
		counting = true;
		startTimer();
	}
}

//funtion to stop the timer
void stopTimer(){
	stateTime=0;
	counting = false;
}
