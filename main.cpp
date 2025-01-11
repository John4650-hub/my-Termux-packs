#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "scroller.hpp"

using namespace ftxui;

ButtonOption Style() {
  auto option = ButtonOption::Animated();
	option.transform = [](const EntryState& s) {
    auto element = text(s.label);
		if (s.focused) {
      element |= bold;
		}
    return element | center | borderEmpty | flex;
  };
	return option;
}

std::vector<Component> GenerateList(){
	std::vector<std::string> items;
  std::ifstream file("list.txt");
	std::string line;
	while (std::getline(file, line)) {
			items.push_back(line);
	}
	std::vector<Component> buttons;
	for (const auto& item : items) {
			auto btn = Button(item, [item] { std::cout << "Clicked: " << item << std::endl; },Style());
			buttons.push_back(Renderer(btn,[item]{return text(item);
		}));
	}
	return buttons;
}

Component MusicList(){
	class Impl : public ComponentBase{
		Component scroll;
		public:
			Impl(){
				scroll=Scroller(Container::Vertical(GenerateList()));
				Add(
#ifdef SCROLLER_INSIDE_WINDOW
          Renderer(scroll, [&] { return window(text(" Test Log: "), vbox(scroll)); })
#else
     scroll
#endif
    );}
	};
	return Make<Impl>();
}


int main(int argc, const char* argv[]) {
  auto musicListWindow = Window({
				.inner=MusicList(),
				.title="My Music",
				.left=0,
				.top=0,
				.width=Terminal::Size().dimx,
				.height=Terminal::Size().dimy/2,
				});

    // Create and run the screen interactive
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(musicListWindow);

    return 0;
}
