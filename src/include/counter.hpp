#ifndef COUNTER_HPP
#define COUNTER_HPP
#include <string>
#include <ftxui/component/screen_interactive.hpp>

extern std::string g_timeCounti{"0"};
extern ftxui::ScreenInteractive g_screen = ftxui::ScreenInteractive::Fullscreen();

void startTimer();
void stopTimer();
void pauseTimer();
#endif
