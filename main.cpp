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

auto label_text = std::make_shared<std::wstring>(L"Quit");
auto label = Button(label_text->c_str(), []() {});

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

std::vector<Component> GenerateList() {
    std::vector<std::string> items;
    std::ifstream file("list.txt");
    std::string line;
    while (std::getline(file, line)) {
        items.push_back(line);
    }
    std::vector<Component> buttons;
    for (const auto& item : items) {
        auto btn = Button(std::wstring(item.begin(), item.end()), [item] { *label_text = std::wstring(item.begin(), item.end()); }, Style());
        buttons.push_back(Renderer(btn, [item] { return text(item); }));
    }
    return buttons;
}

Component MusicList() {
    class Impl : public ComponentBase {
        Component scroll;
    public:
        Impl() {
            scroll = Scroller(Container::Vertical(GenerateList()));
            Add(
#ifdef SCROLLER_INSIDE_WINDOW
                Renderer(scroll, [&] { return window(text(" Test Log: "), vbox(scroll)); })
#else
                scroll
#endif
            );
        }
    };
    return Make<Impl>();
}

void play() {}
void prev() {}
void next() {}

Component PlayerWidget() {
    class Impl : public ComponentBase {
    public:
        Impl() {
            auto play_button_text = std::make_shared<std::wstring>(L"Play");
            auto play_button = Button(play_button_text->c_str(), [play_button_text] {
                if (*play_button_text == L"Play") {
                    play();
                    *play_button_text = L"Pause";
                } else {
                    play();
                    *play_button_text = L"Play";
                }
            });

            auto prev_button = Button(L"Prev", prev);
            auto next_button = Button(L"Next", next);

            auto button_container = Container::Horizontal({
                prev_button,
                play_button,
                next_button,
            });
            Component renderer = Renderer(button_container, [&] {
                return window(text(L"Audio Player"), hbox({
                    prev_button->Render(),
                    play_button->Render(),
                    next_button->Render(),
                }));
            });
            Add(renderer);
        }
    };
    return Make<Impl>();
}

int main(int argc, const char* argv[]) {
    auto screen = ScreenInteractive::Fullscreen();

    auto musicListWindow = Renderer(MusicList(), [](Element inner) {
        return window(text(L"My Music"), inner) | size(WIDTH, LESS_THAN, 80) | size(HEIGHT, LESS_THAN, 15);
    });

    auto audioPlayerWindow = Renderer(PlayerWidget(), [](Element inner) {
        return window(text(L"Audio Player"), inner) | size(WIDTH, LESS_THAN, 80) | size(HEIGHT, LESS_THAN, 10);
    });

    auto windowContainer = Container::Vertical({
        musicListWindow,
        audioPlayerWindow,
        label,
    });

    screen.Loop(windowContainer);
    return 0;
}

