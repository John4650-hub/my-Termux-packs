#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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


int main(int argc, const char* argv[]) {
    using namespace ftxui;
    // Read items from list.txt
    std::vector<std::string> items;
    std::ifstream file("list.txt");
    std::string line;
    while (std::getline(file, line)) {
        items.push_back(line);
    }

    // Create a list of buttons
    std::vector<Component> buttons;
    for (const auto& item : items) {
        buttons.push_back(Button(item, [item] { std::cout << "Clicked: " << item << std::endl; }),Style());
    }

    // Create a vertical container for the buttons
    auto container = Container::Vertical(buttons);

    // Create a renderer for the container
    auto renderer = Renderer(container, [&] {
        Elements elements;
        for (auto& button : buttons) {
            elements.push_back(button->Render());
        }
        return vbox({
            text("Select an item:"),
            separator(),
            vbox(std::move(elements)),
        });
    });

    // Create and run the screen interactive
    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(renderer);

    return 0;
}
