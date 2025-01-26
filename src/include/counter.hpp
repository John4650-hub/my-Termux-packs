#ifndef COUNTER_HPP
#define COUNTER_HPP
#include <string>
#include <ftxui/component/screen_interactive.hpp>
#include <atomic>

extern std::string g_timeCount;
extern std::string g_stateTime;
extern std::atomic g_intit_time;
extern ftxui::ScreenInteractive g_screen; 
extern float timer_progress;
extern int selected_time;

void startTimer();
void stopTimer();
void pauseTimer();
#endif
