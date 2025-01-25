#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/direction.hpp>
#include <array>
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
	auto start_btn = Button("START",startTimer,style());
	auto pause_btn = Button("PAUSE",pauseTimer,style());
	auto stop_btn = Button("STOP",stopTimer,style());
std::array<int, 30> time_options;
for(int i=1;i<time_options.size();++i){
	time_options[i] = i*5;
}

Component timer_Options_list = Container::Horizontal({});
		for(int& val:time_options){
		SliderOption<int> option;
		option.value = &val;
		option.max = 1000;
		option.increment = 5;
		option.direction = Direction::Up;
		timer_Options_list->Add(Slider<int>(option));
		}

timer_Options_list |= size(HEIGHT,GREATER_THAN,20) | border;

	Component TimerWindow = Window({
			.inner = Container::Vertical({
					time_text,
					gaugeProg,
					Container::Horizontal({
							start_btn,
							pause_btn,
							stop_btn
							}),
					timer_Options_list
					}),
			.width=50,
			.height=50
			});

	//screen = ScreenInteractive::Fullscreen();
	g_screen.Loop(TimerWindow);
	return 0;
}
