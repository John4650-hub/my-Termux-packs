TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror
LDFLAGS = -lOpenSLES -lftxui-screen -lftxui-dom -lftxui-component -pthread -lm -ldl
LD_PRELOAD = /system/lib/libpng.so

liblisdir.so: $(FILENAME)
	LD_PRELOAD=/system/lib/libpng.so $(CXX) -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib miniaudio.c scroller.cpp main.cpp -o liblisdir.so $(LDFLAGS)
install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
