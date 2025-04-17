sudo apt-get install libpng-dev -y
gcc -lpng -I./include src/gen_w_mupdf.cpp src/save_to_png.cpp src/mupdf/*.c main.cpp -o main
