TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -fPIC
LDFLAGS += -L$(TERMUX_PREFIX)/lib/ -lpng  -lmupdf -lmupdf-third -lc 
#-lfreetype -lharfbuzz -lz -lopenjp2 -ljbig2dec -ljpeg -llog -lc -ldl
#f.or fpdfium
#-lpdfium
# Rule to create the shared library
pdfviewer.so: main.cpp
	$(CXX) -std=c++17 -I$(TERMUX_PREFIX)/include -I./include $(LDFLAGS)  src/save_to_png.cpp src/gen_w_mupdf.cpp main.cpp -o pdfviewer.so

install: pdfviewer.so
	install pdfviewer.so $(DESTDIR)$(PREFIX)/lib/pdfviewer.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/pdfviewer.so
