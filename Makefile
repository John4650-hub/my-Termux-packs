TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -Oz
LDFLAGS += lftxui-component -lftxui-dom -lftxui-screen -pthread -lm -ldl -lc
PREFIX ?= /usr/local
FILENAME = main.cpp
liblisdir.so: $(FILENAME)
	arm-linux-androideabi-clang++ -march=armv7-a -mfpu=neon -mfloat-abi=softfp -mthumb -fstack-protector-strong $(CFLAGS) -std=c++14 -fno-exceptions -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib -Wl,-rpath=$(TERMUX_PREFIX)/lib -o liblisdir.so scroller.cpp miniaudio.c $(FILENAME) $(LDFLAGS)

install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
