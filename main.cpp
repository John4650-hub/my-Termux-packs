#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
#include "counter.hpp"

using namespace ftxui;
extern std::string timeCount;
extern ScreenInteractive screen screen;

ButtonOption style(){
	auto option = ButtonOption::Animated();
	option.transform = [](const EntryState& s){
		auto element = text(s.label);
		if (s.focused)
			element |= bold;
		return element|center|borderEmpty|flex;
	};
	return option;
}

int main(){
	const 
	auto time_text=Renderer([&](){
			return text(timer_text) | flex ;
			});
	auto start_btn = Button("START",startTimer,style());
	auto pause_btn = Button("PAUSE",pauseTimer,style());
	auto stop_btn = Button("STOP",stopTimer,style());

	Component TimerWindow = Window({
			.inner = Container::Vertical({
					timer_text,
					Container::Horizontal({
							start_btn,
							pause_btn,
							stop_btn
							})
					})
			});

	screen = ScreenInteractive::Fullscreen();
	screen.Loop(TimerWindow);
	return 0;
}
