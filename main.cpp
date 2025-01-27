#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/canvas.hpp"
#include "miniaudio.h"
#include "scroller.hpp"
#include <cmath>

using namespace ftxui;


extern int selected_item_index;
std::string textarea_txt = ".....welcome.to.PlayerTermux...";
std::string rootPath = "";
std::string msg{};
std::shared_ptr<std::wstring> play_button_text;
std::vector<std::string> audioNames;
std::atomic<std::string>  Seek{"0 %"};
std::atomic<std::string>* Seek_ptr = &Seek;
Component musicListWindow;
Component slider;
auto screen = ScreenInteractive::Fullscreen();
bool isPlaying = false;
bool* isPlaying_ptr=&isPlaying;
std::atomic<bool> isPaused{false};
std::atomic<bool>* isPaused_ptr = &isPaused;
std::atomic<int> total_frames{};
std::atomic<int> slider_position{};
int prev_selected_item_index{};
int* prev_selected_item_index_ptr = &prev_selected_item_index;
std::atomic<int> interval{};
std::atomic<int>* interval_ptr=&interval;
std::atomic<int>* total_frames_ptr = &total_frames;
std::atomic<int>* slider_position_ptr=&slider_position;

ma_uint64 FirstFrame;
ma_uint64 currentFrame;
ma_uint64* currentFrame_ptr=&currentFrame;
ma_result result;
ma_decoder decoder;
ma_decoder_config decoderConfig;
ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
ma_device device;
ma_uint64 lengthInFrames;

Event updateSlider = Event::Special("update_slider");

std::atomic<bool> stopFlag(false);

void addLog(std::string a);
void clearInterval();

void init_vars(){
	currentFrame_ptr->store(0);
	total_frames_ptr->store(0);
	slider_position_ptr->store(0);
}
void clearInterval() {
    stopFlag.store(true);
}

void setInterval() {
    std::thread([&]() {
        while (!stopFlag.load()) {
						screen.PostEvent(Event::Custom);
						if (currentFrame.load() == total_frames.load()) {
						*isPlaying_ptr=false;
						ma_device_stop(&device);
            clearInterval();
						addLog("It is done");
						init_vars();
						}
        ma_decoder_get_cursor_in_pcm_frames(&decoder, &currentFrame);
        addLog(std::to_string(currentFrame));
        slider_position_ptr->store((static_cast<double>(currentFrame) / static_cast<double>(total_frames.load())) * 100);
        Seek_ptr->store(std::to_string(slider_position.load()) + " %");
        addLog(std::to_string(currentFrame.load()) + " / " + std::to_string(total_frames.load()) + " = " + std::to_string(slider_position.load()));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
		}).detach();
}


void seek_audio(ma_uint64 position){
	if (static_cast<int>(slider_position.load())>=0 && static_cast<int>(slider_position.load()) <= 100){
	screen.PostEvent(Event::Custom);
	slider_position_ptr->store((static_cast<double>(currentFrame.load())/total_frames.load())*100);
	Seek_ptr->store(std::to_string(slider_position.load())+" %");
	clearInterval();
	ma_device_stop(&device);
	isPaused_ptr->store(true);
	ma_decoder_seek_to_pcm_frame(&decoder,position);
	}
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput,
                   ma_uint32 frameCount) {
  ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}

void addLog(std::string log) {
  textarea_txt = textarea_txt + "\n";
  textarea_txt = textarea_txt + log;
}

std::vector<std::string> getAudioFiles(const std::string& folderPath) {
  std::vector<std::string>
      audioFiles;  // Vector to store valid audio file names
  std::vector<std::string> validExtensions = {
      ".mp3", ".flac", ".wav"};  // List of valid audio file extensions

  // Iterate through the directory specified by folderPath
  for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
    if (entry.is_regular_file()) {  // Check if the entry is a regular file
      std::string fileName =
          entry.path().filename().string();  // Get the file name
      std::string fileExtension =
          entry.path().extension().string();  // Get the file extension

      // Check if the file extension is in the list of valid extensions
      if (std::find(validExtensions.begin(), validExtensions.end(),
                    fileExtension) != validExtensions.end()) {
        audioFiles.push_back(fileName);  // Add the file name to the vector
      }
    }
  }

  return audioFiles;  // Return the vector of valid audio file names
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
  if (!file) {
    msg = ".audioPath.txt file not found!!";
    addLog(msg);
    return list_items;
  }
  std::string line;
  while (std::getline(file, line)) {
    audioPaths.push_back(line);
  }
  std::string audioPath = audioPaths[0];
  rootPath = audioPath;
  for (std::string item : getAudioFiles(audioPath)) {
    auto list_item = Renderer([item] { return text(item); });
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
          Renderer(scroll,
                   [&] { return window(text(" Test Log: "), vbox(scroll)); })
#else
          scroll
#endif
      );
    }
  };
  return Make<Impl>();
}

	
void play() {
  std::string audio_playing = rootPath + "/" + audioNames[selected_item_index];
  musicListWindow->TakeFocus();
	
	if(isPlaying){
		if(prev_selected_item_index==selected_item_index){
			if(isPaused.load()){
				isPaused_ptr->store(false);
				ma_device_start(&device);
				clearInterval();
				setInterval();
				return;
			}else{
				isPaused_ptr->store(true);
				clearInterval();
				ma_device_stop(&device);
				return;
			}
		}
		clearInterval();
    msg = "stopping current audio ...";
		init_vars();
		ma_decoder_uninit(&decoder);
    addLog(msg);
		result = ma_decoder_init_file(audio_playing.c_str(), NULL, &decoder);
	}
	else if(!isPlaying){
		init_vars();
		result = ma_decoder_init_file(audio_playing.c_str(), NULL, &decoder);
		*isPlaying_ptr=true;	
	}

	ma_decoder_get_length_in_pcm_frames(&decoder,&lengthInFrames);
	total_frames_ptr->store((int)lengthInFrames);
	addLog("totalFrames: " + std::to_string(total_frames.load()));
	addLog("sampleRate: "+std::to_string(decoder.outputSampleRate));
	addLog("Format: "+std::to_string(decoder.outputFormat));
	addLog("outputChannels: "+ std::to_string(decoder.outputChannels));
  deviceConfig.playback.format = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate = decoder.outputSampleRate;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = &decoder;

    if (result != MA_SUCCESS) {
      msg = "could not play the audio file";
				addLog(msg);
				return;
    }

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
      msg = "Failed to open playback device.\n";
      addLog(msg);
      ma_decoder_uninit(&decoder);
      return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
      msg = "Failed to start playback device.\n";
      addLog(msg);
      ma_device_uninit(&device);
      ma_decoder_uninit(&decoder);
      return;
    }
    msg = "audio file loaded, starting.... ";
		addLog(msg);
		ma_decoder_get_cursor_in_pcm_frames(&decoder,&FirstFrame);
		interval_ptr->store((total_frames.load() - FirstFrame)/99);
		setInterval();
		*prev_selected_item_index_ptr=selected_item_index;
}

void prev() {
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
      play_button_text = std::make_shared<std::wstring>(L"Play");
      auto play_button = Button(
          play_button_text->c_str(),
          [&] {
play();},
          Style());

      auto prev_button = Button("Back", prev, Style());
      auto next_button = Button("Next", next, Style());
      Component button_container = Container::Horizontal({
          Renderer(prev_button,
                   [prev_button] { return prev_button->Render(); }),
          Renderer(play_button,
                   [play_button] { return play_button->Render() | flex; }),
          Renderer(next_button,
                   [next_button] { return next_button->Render(); }),
      });
      Add(button_container);
    }
  };
  return Make<Impl>();
}

Component logsWindow() {
  class Impl : public ComponentBase {
   public:
    Impl() {
      auto textarea_log = Input(&textarea_txt);
      Component txtlogs =
          Container::Vertical({Renderer(textarea_log, [textarea_log] {
            return vbox({text("Audio logs"), separator(),
                         textarea_log->Render() | flex |
                             size(HEIGHT, EQUAL, 50)}) |
                   border | bgcolor(Color::Black);
          })});
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
  auto exit_button = Button("Exit", [&] {
    ma_decoder_uninit(&decoder);
    ma_device_uninit(&device);
    screen.Exit();
  },Style());

	slider = Slider(&Seek,&slider_position,0,100,1);
  musicListWindow = Window({
      .inner = MusicList(),
      .title = "My Music",
      .width = Terminal::Size().dimx,
      .height = Terminal::Size().dimy*0.6,
  });
auto audioPlayerWindow = Window({
    .inner = Container::Vertical({
        CatchEvent(Renderer(slider, [&] {
            return slider->Render() | focus;
        }), [&](Event event) {
            if (event == Event::ArrowLeft)
						{
						*currentFrame_ptr=FirstFrame;
						seek_audio(0);
						return true;
						}else if (event == Event::ArrowRight) {
								*currentFrame_ptr +=interval.load();
								seek_audio(currentFrame);
								return true;
						}
            return false;
        }),
        PlayerWidget()
    }),
    .width = Terminal::Size().dimx,
    .height = Terminal::Size().dimy * 0.65,
});
   auto logout = Window({
      .inner = logsWindow(),
      .title = "Audio logs",
      .width = Terminal::Size().dimx,
      .height = Terminal::Size().dimy*0.59});
	auto exitBtn = Window({
			.inner = Renderer(exit_button,[exit_button]{return exit_button->Render();}),
			.width=Terminal::Size().dimx,
			.height = Terminal::Size().dimy*0.25,
			});
  auto windowContainer =
      Container::Vertical({exitBtn,logout,musicListWindow,audioPlayerWindow});
  screen.Loop(windowContainer);
	ma_decoder_uninit(&decoder);
  ma_device_uninit(&device);
  return 0;
}
