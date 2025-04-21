#include "gen_w_mupdf.hpp"
#include <argparse/argparse.hpp>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[]) {
  [[maybe_unused]] float  width{100.0f};
  [[maybe_unused]] int page_number{1};
  [[maybe_unused]] int height{1};
  [[maybe_unused]] int total_pages{1};
  [[maybe_unused]] int incr{1};
  

  argparse::ArgumentParser program("pdfViewer using pdfmium", "135.0.7087.0",
                                   argparse::default_arguments::help, false);

  program.add_argument("-i")
    .required()
    .help("Name of the pdf file");

program.add_argument("-T","--total-pages")
  .flag()
  .help("Get the total pages in the current pdf file");

  program.add_argument("-p", "--page")
      .default_value(1)
      .help("page to view")
      .scan<'i',int>();

  program.add_argument("-s", "--height")
      .default_value(1)
      .help("factor by which to scale the page image")
      .scan<'i',int>();
program.add_argument("-z", "--width")
      .default_value(100.0f)
      .help("factor by which to width the page image")
      .scan<'f',float>();
  program.add_argument("-c", "--incr")
      .default_value(1)
      .help("factor by which to scale the page image")
      .scan<'i',int>();


  try {
    program.parse_args(argc, argv);
      auto arg = program.present("-i");
      const char *fname = arg->c_str();
      bool is_get_total_pages=program.get<bool>("--total-pages");
      if(is_get_total_pages){
        std::cout<<mupdf_get_total_pages(fname);
      }else{
      try{
      page_number=program.get<int>("--page");
      if (program.is_used("--height")){
        height=program.get<int>("--height");
      }
      if (program.is_used("--width")){
        width =program.get<float>("--width");
      }
      if(program.is_used("--incr")){
        incr=program.get<int>("--incr");
      }
      mupdf_gen_page(fname,page_number,width,height,incr);
      } catch(const std::runtime_error &e){
          std::cerr << e.what() << "\n";
          std::cerr << program << "\n";
          std::exit(1);
          return 1;
      }
      }
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << "\n";
    std::cerr << program << "\n";
    std::exit(1);
    return 1;
  }
  return 0;
}
