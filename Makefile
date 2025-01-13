TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -Oz
LDFLAGS += -lOpenSLES -lsndfile -lftxui-screen -lftxui-dom -lftxui-component -pthread
PREFIX ?= /usr/local
FILENAME = main.cpp
liblisdir.so: $(FILENAME)
	arm-linux-androideabi-clang++ -march=armv7-a -mfpu=neon -mfloat-abi=softfp -mthumb -fstack-protector-strong $(CFLAGS) -std=c++14 -fno-exceptions -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib -Wl,-rpath=$(TERMUX_PREFIX)/lib -Wl,--enable-new-dtags -Wl,--as-needed -Wl,-z,relro,-z,now -o liblisdir.so scroller.cpp play_audio.cpp $(FILENAME) $(LDFLAGS)

install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
