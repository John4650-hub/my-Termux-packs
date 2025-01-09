TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -Oz

liblisdir.so: main.cpp
	$(CXX) -I/usr/include/c++/13 -I/usr/include/x86_64-linux-gnu/c++/13 -I/usr/include/c++/13/backward -I/usr/include -L/usr/lib/x86_64-linux-gnu main.cpp -o liblisdir.so -lncurses
install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
