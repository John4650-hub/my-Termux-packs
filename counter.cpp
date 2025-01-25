#include <iostream>
#include <chrono>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

std::atomic<bool> counting{false};
std::atomic<int> stateTime{10};
std::string g_stateTime = "";
std::string g_timeCount{std::to_string(stateTime)};
std::atomic<int> init_time{10};
float timer_progress{};
ftxui::ScreenInteractive g_screen=ftxui::ScreenInteractive::Fullscreen();

// function to start timer
void startTimer(){
	if(counting)
		return;
	else if(g_stateTime.length()>0)
		stateTime=std::stoi(g_stateTime.c_str());
	init_time=stateTime;
	counting = true;
	std::thread([&](){
	while(stateTime>0 && counting == true){
		g_screen.PostEvent(ftxui::Event::Custom);
		stateTime-=1;
		timer_progress = 1.0f - (static_cast<double>(stateTime)/init_time);
		g_timeCount = std::to_string(stateTime);
		std::this_thread::sleep_for(std::chrono::seconds(1000));
		}
}
).detach();
}

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
	*ptr=init_time;
	g_timeCount=std::to_string(stateTime);
	g_screen.Post(ftxui::Event::Custom);
	counting = false;
}
