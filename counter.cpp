#include <iostream>
#include <chrono>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

std::atomic<bool> counting{true};
std::atomic<int> stateTime{10000};
std::string g_timeCount{std::to_string(stateTime)};
float timer_progress{};
ftxui::ScreenInteractive g_screen=ftxui::ScreenInteractive::Fullscreen();

void startTimer(){
	counting = true;
	std::thread([&](){
	while(stateTime>0 && counting == true){
		g_screen.PostEvent(ftxui::Event::Custom);
		stateTime-=1;
		timer_progress = 1.0f - (static_cast<double>(stateTime)/10);
		g_timeCount = std::to_string(stateTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
std::atomic<int>* ptr = &stateTime;
void stopTimer(){
	*ptr=10000;
	g_timeCount=std::to_string(stateTime);
	g_screen.Post(ftxui::Event::Custom);
	counting = false;
}
