#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <ftxui/component/event.hpp>
#include "counter.hpp"

bool counting{true};
int stateTime{5};
int* ptr = &stateTime;
extern std::string timeCount;
extern ftxui::ScreenInteractive screen;

void startTimer(){
	std::thread t(timer,std::ref(stateTime));
	t.detach();
}
// function to start timer
void timer(int& current_time){
	while(current_time>=0 && counting == true){
		screen.PostEvent(ftxui::Event::Custom);
		current_time-=1;
		*ptr = current_time;
		timeCount = std::to_string(current_time);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
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
	*ptr = 0;
	counting = false;
}
