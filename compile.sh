sudo apt-get install libpng -y
gcc -lpng -I./include src/*.cpp src/mupdf/*.c main.cpp -o main
