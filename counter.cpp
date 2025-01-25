#include <iostream>
#include <chrono>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

std::atomic<bool> counting{true};
std::atomic<int> stateTime{10};
std::string g_timeCount{std::to_string(stateTime)};
ftxui::ScreenInteractive g_screen=ftxui::ScreenInteractive::Fullscreen();

void startTimer(){
	std::thread([&](){
	while(stateTime>0 && counting == true){
		g_screen.PostEvent(ftxui::Event::Custom);
		stateTime-=1;
		g_timeCount = std::to_string(stateTime);
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
	stateTime=10;
	g_screen.Post(ftxui::Event::Custom);
	counting = false;
}
