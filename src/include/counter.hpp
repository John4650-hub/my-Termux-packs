#ifndef COUNTER_HPP
#define COUNTER_HPP
#include <string>
#include <ftxui/component/screen_interactive.hpp>
#include <vector>

extern std::string g_timeCount;
extern std::string g_stateTime;
extern ftxui::ScreenInteractive g_screen; 
extern float timer_progress;
extern int selected_item;
extern std::vector<std::string> time_options;

void startTimer();
void stopTimer();
void pauseTimer();
#endif
