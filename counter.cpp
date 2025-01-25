#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

std::atomic<bool> counting{true};
std::atomic<int> stateTime{10};
extern std::string timeCount;
extern ftxui::ScreenInteractive screen;

void timer(int& current_time){
	while(current_time>=0 && counting == true){
		screen.PostEvent(ftxui::Event::Custom);
		current_time-=1;
		timeCount = std::to_string(current_time);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void startTimer(){
	std::thread t(timer,std::ref(stateTime));
	t.detach();
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
