TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -Oz
LDFLAGS = -lftxui-screen -lftxui-dom -lftxui-component #-pthread -lc
FILENAME = main.cpp
liblisdir.so: $(FILENAME)
	$(CXX) -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib $(FILENAME) -o liblisdir.so $(LDFLAGS)
install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
