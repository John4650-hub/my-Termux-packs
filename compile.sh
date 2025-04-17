
sudo apt-get install libpng-dev -y
mkdir argparse
cd argparse
curl -LO "https://raw.githubusercontent.com/p-ranav/argparse/refs/heads/master/include/argparse/argparse.hpp"
cd ..
mv argparse include/
files=$(ls ./src/mupdf-src/* | tr '\n' ' ')
gcc -lpng -lstdc++ -I./include src/gen_w_mupdf.cpp src/save_to_png.cpp $files main.cpp -o main
