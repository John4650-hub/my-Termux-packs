#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "scroll.cpp"

using namespace ftxui;

// #define SCROLLER_INSIDE_WINDOW       1

Component TestWindowContent() {
 class Impl : public ComponentBase {
  Component scroll, log;
  public:
   Impl() {
    log = Container::Vertical({});
    for (uint i = 0; i < 100; i++) {
     std::string l("Log line" + std::to_string(i));
     log->Add(Renderer([l] { return text(l); }));
    }
    scroll = Scroller(log);
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
};

int main() {
 auto window = Window({
  .inner = TestWindowContent(),
  .title = " Test App ",
  .left = 0,
  .top = 0,
  .width = Terminal::Size().dimx,
  .height = Terminal::Size().dimy,
  .resize_left = false,
  .resize_right = false,
  .resize_top = false,
  .resize_down = false,
  // .draggable = false
 });

 auto screen = ScreenInteractive::Fullscreen();
 screen.Loop(window);

 return EXIT_SUCCESS;
}
