TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -fPIC
LDFLAGS += -L$(TERMUX_PREFIX)/lib/ -lpdfium -lpng

# Rule to create the shared library
pdfviewer.so: main.cpp
	$(CXX) -I$(TERMUX_PREFIX)/include $(LDFLAGS) main.cpp -o pdfviewer.so

install: pdfviewer.so
	install pdfviewer.so $(DESTDIR)$(PREFIX)/lib/pdfviewer.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/pdfviewer.so
