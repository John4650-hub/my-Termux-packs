#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, const char* argv[]) {
    using namespace ftxui;

    // Read items from list.txt
    std::vector<std::wstring> items;
    std::wifstream file("list.txt");
    std::wstring line;
    while (std::getline(file, line)) {
        items.push_back(line);
    }

    // Create a list of buttons
    std::vector<Component> buttons;
    for (const auto& item : items) {
        buttons.push_back(Button(item, [] { std::wcout << L"Clicked!" << std::endl; }));
    }

    // Create a vertical container for the buttons
    auto container = Container::Vertical(buttons);

    // Create a renderer for the container
    auto renderer = Renderer(container, [&] {
        return vbox({
            text(L"Select an item:"),
            separator(),
            vbox(buttons | transformed([](auto button) { return button->Render(); })),
        });
    });

    // Create and run the screen interactive
    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(renderer);

    return 0;
}
