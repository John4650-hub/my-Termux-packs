TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror
LDFLAGS = -lOpenSLES -pthread -lm -ldl -L./oboe/libs -loboe
LDFLAGS += -L./src/lib/ -lavformat -lavcodec -lswresample -lavutil
liblisdir.so: $(FILENAME)
	$(CXX) -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib -I./oboe/include -I./src/include main.cpp -o liblisdir.so $(LDFLAGS)
install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
