TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror
LDFLAGS = -loboe -lavformat -lavcodec -lswresample -lavutil -lOpenSLES -pthread -lm -ldl

liblisdir.so: $(FILENAME)
	$(CXX) -g -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib -I./src/include/ player.cpp main.cpp -o liblisdir.so $(LDFLAGS)

install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
