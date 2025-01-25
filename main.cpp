#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
#include "counter.hpp"

using namespace ftxui;

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
	auto time_text=Renderer([&]{
			return text(g_timeCount) | flex |center ;
			});
	auto gaugeProg = Renderer([&]{
			return gauge(timer_progress)| color(Color::Red)|border;
			});
	auto start_btn = Button("START",startTimer,style())flex;
	auto pause_btn = Button("PAUSE",pauseTimer,style())|flex;
	auto stop_btn = Button("STOP",stopTimer,style())| flex;


	Component TimerWindow = Window({
			.inner = Container::Vertical({
					time_text,
					gaugeProg,
					Container::Horizontal({
							start_btn,
							pause_btn,
							stop_btn
							})
					})
			});

	//screen = ScreenInteractive::Fullscreen();
	g_screen.Loop(TimerWindow);
	return 0;
}
