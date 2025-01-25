#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
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
	selected_item = 10;
	auto time_text=Renderer([&]{
			return text(g_timeCount) | flex |center ;
			});
	auto gaugeProg = Renderer([&]{
			return gauge(timer_progress)| color(Color::Red)|border;
			});
	auto start_btn = Button("START",startTimer,style());
	auto pause_btn = Button("PAUSE",pauseTimer,style());
	auto stop_btn = Button("STOP",stopTimer,style());

for(int i=1;i<100;++i){
	time_options.push_back(std::to_string(i*5));
}

auto timer_Options_list =  Menu(&time_options, &selected_time);

auto time_list_render = Renderer(timer_Options_list,[&]{
		return vbox({
				timer_Options_list->Render(),
				text("selected time: "+ time_options[selected_time]),
				}) | border;
		});

	Component TimerWindow = Window({
			.inner = Container::Vertical({
					time_text,
					gaugeProg,
					Container::Horizontal({
							start_btn,
							pause_btn,
							stop_btn
							}),
					time_list_render
					}),
			.width=50,
			.height=50
			});

	//screen = ScreenInteractive::Fullscreen();
	g_screen.Loop(TimerWindow);
	return 0;
}
