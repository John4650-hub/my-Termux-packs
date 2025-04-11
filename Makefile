TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -fPIC
LDFLAGS += -L$(TERMUX_PREFIX)/lib/ #not needed
#-lleptonica  -lwebp -ljpeg -lopenjp2

# Rule to create the shared library
check_lib.so: main.cpp
	$(CXX) -I$(TERMUX_PREFIX)/include $(LDFLAGS) check_lib.cpp -o check_lib.so

install: check_lib.so
	install check_lib.so $(DESTDIR)$(PREFIX)/lib/pdfviewer.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/check_lib.so
