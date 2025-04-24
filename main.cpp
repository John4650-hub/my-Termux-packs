#include "gen_w_mupdf.hpp"
#include <argparse/argparse.hpp>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[]) {
  [[maybe_unused]] int page_number{1};
  [[maybe_unused]] int total_pages{1};
  

  argparse::ArgumentParser program("pdfViewer using mupdf", "1.24.10",
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
      mupdf_gen_page(fname,page_number);
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
