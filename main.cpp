#include "gen_image.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <integer>" << std::endl;
        return 1;
    }
    int page_number = std::atoi(argv[1]);
    int scale_n = std::atoi(argv[2]);
    gen_new_page(file_name,page_number,img_scale);
    return 0;
}
