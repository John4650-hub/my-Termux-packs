#!/bin/bash

mu_src=$(ls src/mupdf-src/* | tr '\n' ' ')
first_part='$(CXX) -static -I$(TERMUX_PREFIX)/include -I./include $(LDFLAGS)  src/save_to_png.cpp'
last_part='src/gen_w_mupdf.cpp main.cpp -o pdfviewer.so'
full_commad="\t$first_part $mu_src $last_part"
sed -i "8 a\
  $full_commad" Makefile

