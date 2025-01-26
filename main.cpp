#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "counter.hpp"
#include <vector>
#include <atomic>
#include <string>

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
	std::atomic<int>* ptr_  = &g_init_time;
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
auto timer_Options_list =  Menu(&time_options, &selected_time)|CatchEvent([&](Event event) {
        if (event == Event::ArrowDown) {
				g_screen.PostEvent(Event::Custom);
            selected_time = (selected_time + 1) % time_options.size();
						ptr_->store(std::stoi(time_options[selected_time]));
            return true;
        }
        if (event == Event::ArrowUp) {
				g_screen.PostEvent(Event::Custom);
            selected_time = (selected_time + time_options.size() - 1) % time_options.size();
						ptr_->store(std::stoi(time_options[selected_time]));
            return true;
        }
        return false;
    });
auto container = Container::Vertical({
		timer_Options_list,
		});
auto time_list_render = Window({
		.inner = Renderer(container,[&]{
		return vbox({
				timer_Options_list->Render() | vscroll_indicator|frame|size(HEIGHT,LESS_THAN,10)
				}) | border;
		}),
		.top = 10,
		.width = 10,
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
auto renderer = Renderer([&] {
    return vbox({
      text("Message: " + std::to_string(g_init_time)),
      text("Value: " + std::to_string(g_init_time))
    }) | border;
  });
auto windows = Container::Vertical({
		TimerWindow,
		time_list_render,
		renderer
		});

	//screen = ScreenInteractive::Fullscreen();
	g_screen.Loop(windows);
	return 0;
}
