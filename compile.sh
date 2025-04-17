sudo apt-get install libpng-dev -y
files=$(ls ./src/mupdf-src/* | tr '\n' ' ')
gcc -lpng -I./include src/gen_w_mupdf.cpp src/save_to_png.cpp "$files" main.cpp -o main
