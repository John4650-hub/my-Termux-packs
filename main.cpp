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
bool isPlaying = false;
Component scroll;

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

void play() {
	/**if(!(isPlaying)){
		playAudio(selected_item_text);
		isPlaying=true;
	} else{
		pause();
	}
	**/
}
void prev(){
	scroll->OnEvent(Event::Character('k'));
}
void next() {
	scroll->onEvent(Event::Character('j'));
}

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
            },Style());

            auto prev_button = Button("Back", prev,Style());
            auto next_button = Button("Next", next,Style());
/**						auto prev_canvas = Canvas(10,10);
						prev_canvas.DrawText(0,0,"<",Color::Red);

						auto next_canvas = Canvas(10,10);
						next_canvas.DrawText(0,0,">",Color::Red);
**/
            Component button_container = Container::Horizontal({
								Renderer(prev_button, [prev_button] { return prev_button->Render();}),
                Renderer(play_button, [play_button] { return play_button->Render() | flex; }),
                Renderer(next_button, [next_button]{ return next_button->Render();}),
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
