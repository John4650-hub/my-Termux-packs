#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/dom/canvas.hpp"
#include <ftxui/component/event.hpp>
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "scroller.hpp"
#include "miniaudio.h"

using namespace ftxui;
extern int selected_item_index;
std::string textarea_txt="Welcome ...";
std::string rootPath="";
int prev_selected_item_index{-1};
std::string msg{};
std::vector<std::string> audioNames;
Component musicListWindow;
auto screen = ScreenInteractive::Fullscreen();
bool isPlaying = false;
bool playedBefore=false;

ma_result result;
ma_sound sound;
ma_engine engine;

void addLog(std::string log){
	textarea_txt=textarea_txt+"\n";
	textarea_txt=textarea_txt + log;
}

std::vector<std::string> getAudioFiles(const std::string& folderPath) {
    std::vector<std::string> audioFiles; // Vector to store valid audio file names
    std::vector<std::string> validExtensions = {".mp3", ".flac", ".wav"}; // List of valid audio file extensions

    // Iterate through the directory specified by folderPath
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) { // Check if the entry is a regular file
            std::string fileName = entry.path().filename().string(); // Get the file name
            std::string fileExtension = entry.path().extension().string(); // Get the file extension

            // Check if the file extension is in the list of valid extensions
            if (std::find(validExtensions.begin(), validExtensions.end(), fileExtension) != validExtensions.end()) {
                audioFiles.push_back(fileName); // Add the file name to the vector
            }
        }
    }

    return audioFiles; // Return the vector of valid audio file names
}

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
    std::vector<std::string> audioPaths;
    std::vector<Component> list_items;
    std::ifstream file("/data/data/com.termux/files/home/.audioPath.txt");
		if(!file){
			msg=".audioPath.txt file not found!!";
			addLog(msg);
			return list_items;
		}
    std::string line;
    while (std::getline(file, line)) {
        audioPaths.push_back(line);
    }
		std::string audioPath = audioPaths[0];
		rootPath=audioPath;
    for (std::string item : getAudioFiles(audioPath)) {
        auto list_item = Renderer([item]{ return text(item);});
				list_items.push_back(list_item);
				audioNames.push_back(item);
    }
    return list_items;
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

void play() {
	std::string audio_playing = rootPath+"/"+audioNames[selected_item_index];
	musicListWindow->TakeFocus();
	if(isPlaying==false){
		result = ma_sound_init_from_file(&engine, audio_playing.c_str(), 0, NULL, NULL, &sound);
		if(selected_item_index == prev_selected_item_index*2){
			if(playedBefore)
				ma_sound_uninit(&sound);
			result = ma_sound_init_from_file(&engine, audio_playing.c_str(), 0, NULL, NULL, &sound);
			prev_selected_item_index=selected_item_index;
		}
		if (result != MA_SUCCESS) {
				msg="could not play the audio file";
				addLog(msg);
				return;
				}
		msg="wait ...";
		addLog(msg);
		isPlaying=true;
		playedBefore=true;
		ma_sound_start(&sound);
		msg="audio file loaded, starting... ";
		addLog(msg);
	}
	else if(isPlaying==true){
		ma_sound_stop(&sound);
		isPlaying=false;
	}
}

void prev(){
	musicListWindow->TakeFocus();
	screen.PostEvent(Event::ArrowUp);
}
void next() {
	musicListWindow->TakeFocus();
	screen.PostEvent(Event::ArrowDown);
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

Component logsWindow(){
	class Impl : public ComponentBase{
		public:
			Impl(){
				auto textarea_log = Input(&textarea_txt);
				Component txtlogs = Container::Vertical({Renderer(textarea_log,[textarea_log]{
						return vbox({
								text("Input"),
								separator(),
								textarea_log->Render() | flex|size(HEIGHT,EQUAL,50)
								}) | border;
						})
						});
				Add(txtlogs);
			}
	};
	return Make<Impl>();
}

int main() {
	if (result != MA_SUCCESS) {
		msg = "Failed to initialize the engine.";
		addLog(msg);
	}
	auto label = Button("Exit", [&]{
			screen.ExitLoopClosure();
			});

	musicListWindow = Window({
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
			.top=15,
			.width=Terminal::Size().dimx,
			.height=Terminal::Size().dimy/3,
			});
auto logout = Window({
			.inner=logsWindow(),
			.title="my logs",
			.left=0,
			.top=20,
			.width=Terminal::Size().dimx,
			.height=Terminal::Size().dimy/1.5,
			});

	auto windowContainer = Container::Stacked({
			musicListWindow,
			audioPlayerWindow,
			logout
			});

screen.Loop(windowContainer);
return 0;
}

