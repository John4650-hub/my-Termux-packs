#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/dom/canvas.hpp"
#include "ftxui/component/component_base.hpp"
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

            auto prev_button = Button("<", prev);
            auto next_button = Button(">", next);
						auto prev_canvas = Canvas(10,10);
						prev_canvas.DrawText(0,0,"<",Color::Red);

						auto next_canvas = Canvas(10,10);
						next_canvas.DrawText(0,0,">",Color::Red);
            Component button_container = Container::Horizontal({
								Renderer(prev_button, [prev_canvas] { return canvas(prev_canvas);}),
                Renderer(play_button, [play_button] { return play_button->Render() | flex; }),
                Renderer(next_button, [next_canvas]{ return canvas(next_canvas);}),
            });
      Add(button_container);
        }
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

	auto audioPlayerWindow = Window({
			.inner=PlayerWidget(),
			.left=0,
			.top=20,
			.width=Terminal::Size().dimx,
			.height=Terminal::Size().dimy/3,
			});

	auto windowContainer = Container::Stacked({
			musicListWindow,
			audioPlayerWindow,
			label
			});

auto screen = ScreenInteractive::Fullscreen();
screen.Loop(windowContainer);
return 0;
}
