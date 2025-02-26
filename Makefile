TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror
LDFLAGS = -loboe -lavformat -lavcodec -lswresample -lavutil -lOpenSLES -ldl -lftxui-screen -lftxui-dom -lftxui-component -pthread -lm

timer.so: $(FILENAME)
	$(CXX) -g -I$(TERMUX_PREFIX)/include -L$(TERMUX_PREFIX)/lib -I./src/include player.cpp counter.cpp main.cpp -o timer.so $(LDFLAGS)

install: timer.so
	install timer.so $(DESTDIR)$(PREFIX)/lib/timer.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/timer.so
