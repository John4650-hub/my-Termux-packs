#include <iostream>
#include <chrono>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"
#include <vector>

std::atomic<bool> counting{false};
std::atomic<int> stateTime{10};
std::string g_stateTime = "";
std::string g_timeCount{std::to_string(stateTime)};
int selected_time = 0;
std::vector<std::string> time_options;

std::atomic<int>* ptr = &stateTime;
float timer_progress{};
std::atomic<int> init_time{10};
ftxui::ScreenInteractive g_screen=ftxui::ScreenInteractive::Fullscreen();

// function to start timer
void startTimer(){
	if(counting.load())
		return;
	//init_time.store(std::stoi(time_options[selected_time].c_str()));
	counting.store(true);
	ptr->store(init_time.load());
	std::thread([&](){
	while(stateTime.load()>0 && counting.load() == true){
		g_screen.PostEvent(ftxui::Event::Custom);
		stateTime.fetch_sub(1);
		timer_progress = 1.0f - (static_cast<double>(stateTime.load())/init_time.load());
		g_timeCount = std::to_string(stateTime.load());
		std::this_thread::sleep_for(std::chrono::seconds(1));
		}
}
).detach();
}

//function to pause the timer
void pauseTimer(){
	if(counting)
		counting.store(false);
	else{
		counting.store(true);
		std::thread([&](){
  		while(stateTime.load()>0 && counting.load() == true){
				g_screen.PostEvent(ftxui::Event::Custom);
				stateTime.fetch_sub(1);
				timer_progress = 1.0f - (static_cast<double>(stateTime.load())/init_time.load());
				g_timeCount = std::to_string(stateTime.load());
				std::this_thread::sleep_for(std::chrono::seconds(1));
				}
		}
		).detach();

	}
}

//funtion to stop the timer
void stopTimer(){
	ptr->store(init_time.load());
	g_timeCount=std::to_string(stateTime);
	g_screen.Post(ftxui::Event::Custom);
	counting.store(false);
}
