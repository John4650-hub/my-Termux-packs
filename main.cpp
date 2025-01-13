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
#include "play_audio.hpp"

using namespace ftxui;

AudioPlayer Aplayer;
std::string rootPath="";

auto screen = ScreenInteractive::Fullscreen();
auto label_text = std::make_shared<std::wstring>(L"Quit");
auto label = Button(label_text->c_str(), []() {});
bool isPlaying = false;
Component musicListWindow;

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
    std::ifstream file("/data/data/com.termux/files/home/.audioPath.txt");
    std::string line;
    while (std::getline(file, line)) {
        audioPaths.push_back(line);
    }
		std::string audioPath = audioPaths[0];
		rootPath=audioPath;
    std::vector<Component> list_items;
    for (std::string item : getAudioFiles(audioPath)) {
        auto list_item = Renderer([item]{ return text(item);});
				list_items.push_back(list_item);
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
void changeAudioStream(){
	Aplayer.playAudio(rootPath +"/"+ selected_item_text);
	isPlaying=true;
}
void play() {
	if(isPlaying==false){
		Aplayer.playAudio(rootPath +"/"+ selected_item_text);
		isPlaying=true;
	}
	if(isPlaying==true){
		isPlaying=false;
	}
}
void prev(){
	musicListWindow->TakeFocus();
	screen.PostEvent(Event::ArrowUp);
	changeAudioStream();
}
void next() {
	musicListWindow->TakeFocus();
	screen.PostEvent(Event::ArrowDown);
	changeAudioStream();
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
	Aplayer.setStreamType(SL_ANDROID_STREAM_MEDIA);
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
			.top=20,
			.width=Terminal::Size().dimx,
			.height=Terminal::Size().dimy/3,
			});

	auto windowContainer = Container::Stacked({
			musicListWindow,
			audioPlayerWindow,
			label
			});

screen.Loop(windowContainer);
return 0;
}

/**
 * #include "audio_player.h"

// Create an instance of AudioPlayer
AudioPlayer Aplayer;

// Play an audio file
Aplayer.play("path/to/audio/file.mp3");
if (!Aplayer.getError().empty()) {
    std::string error = Aplayer.getError();
    // Handle the error accordingly
}

// Set audio stream type
Aplayer.setStreamType(SL_ANDROID_STREAM_MEDIA);

// Get current stream type
SLint32 currentStream = Aplayer.getCurrentStream();

// Pause or resume playback
Aplayer.pause();

// Get current progress
float progress = Aplayer.getProgress();

// Destroy the player when closing the TUI
Aplayer.destroy();
**/
