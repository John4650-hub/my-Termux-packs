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

std::atomic<int>* ptr = &stateTime;
float timer_progress{};
std::atomic<int> init_time{10};
ftxui::ScreenInteractive g_screen=ftxui::ScreenInteractive::Fullscreen();

// function to start timer
void startTimer(){
	if(counting)
		return;
	init_time = std::stoi(time_options[selected_time].c_str());
	counting = true;
	ptr->store(init_time.load());
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
void stopTimer(){
	ptr->store(init_time.load());
	g_timeCount=std::to_string(stateTime);
	g_screen.Post(ftxui::Event::Custom);
	counting = false;
}
