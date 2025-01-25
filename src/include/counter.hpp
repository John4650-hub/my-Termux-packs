#ifndef COUNTER_HPP
#define COUNTER_HPP
#include <string>
#include <ftxui/component/screen_interactive.hpp>

ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
std::string timeCount{"0"};
void startTimer();
void stopTimer();
void pauseTimer();
void timer(int& tim);
#endif
