TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror
# Rule to create the object file
main.o: main.cpp
	$(CXX) -c main.cpp -o main.o

# Rule to create the shared library
liblisdir.so: main.o
	$(CXX) main.o $(TERMUX_PREFIX)/lib/libjvm.so -o liblisdir.so
install: liblisdir.so
	install liblisdir.so $(DESTDIR)$(PREFIX)/lib/liblisdir.so

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/liblisdir.so
