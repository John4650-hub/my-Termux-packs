#include "gen_image.hpp"
#include "extract_text.hpp"
#include <argparse/argparse.hpp>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[]) {
  [[maybe_unused]] bool text_extraction_mode{false};
  [[maybe_unused]] int page_number{1};
  [[maybe_unused]] float scale_factor{1.0f};
  [[maybe_unused]] float x_coordinate{};
  [[maybe_unused]] float y_coordinate{};
  [[maybe_unused]] float width_crop_dim{};
  [[maybe_unused]] float height_crop_dim{};

  argparse::ArgumentParser program("pdfViewer using pdfmium", "135.0.7087.0",
                                   argparse::default_arguments::help, false);

  program.add_argument("-i")
    .required()
    .help("Name of the pdf file");

program.add_argument("-m", "--mode")
    .required()
    .help("add to change mode to text extraction mode");


  program.add_argument("-p", "--page")
      .store_into(page_number
          )
      .help("page to view");

  program.add_argument("-s", "--scale_factor")
      .store_into(scale_factor)
      .help("factor by which to scale the page image");

  program.add_argument("-x","--x_coordinate")
    .store_into(x_coordinate)
    .help("x coordinate when selecting text");
  program.add_argument("-y","--y_coordinate")
    .store_into(y_coordinate)
    .help("y coordinate when selecting text");
  program.add_argument("-w","--width")
    .store_into(width_crop_dim)
    .help("width span by selection box on image");

  program.add_argument("-h","--height")
    .store_into(height_crop_dim)
    .help("height span by selection box on image");

  try {
    program.parse_args(argc, argv);
    if (auto arg = program.present("-i")) {
      const char *fname = arg->c_str();
      if(program.present("-m")){
        getSelectedText(fname,x_coordinate,y_coordinate,width_crop_dim,height_crop_dim);//pass image name
        return 0;
      }
      gen_page_image(fname,page_number,scale_factor);
    }
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << "\n";
    std::cerr << program << "\n";
    return 1;
  }
  return 0;
}
