#ifndef COUNTER_HPP
#define COUNTER_HPP
#include <string>
#include <ftxui/component/screen_interactive.hpp>

extern std::string g_timeCount;
extern std::string g_stateTime;
extern ftxui::ScreenInteractive g_screen; 
extern float timer_progress;

void startTimer();
void stopTimer();
void pauseTimer();
#endif
