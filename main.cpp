#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include "counter.hpp"
#include <vector>

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
	std::vector<std::string> time_options;
	int selected_time = 0;
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
auto container = Container::Vertical({
		timer_Options_list,
		});
auto time_list_render = Renderer(container,[&]{
		return vbox({
				timer_Options_list->Render() | vscroll_indicator|frame|size(HEIGHT,GREATER_THAN,10)
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
							})
					}),
			.width=50
			});
auto windows = Container::Stacked({
		TimerWindow,
		time_list_render
		});
	//screen = ScreenInteractive::Fullscreen();
	g_screen.Loop(windows);
	return 0;
}
