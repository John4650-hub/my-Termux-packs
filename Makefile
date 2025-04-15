TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror

# Compile main.c to main.o
main.o: main.c
	$(CXX) $(CFLAGS) -I$(TERMUX_PREFIX)/include -c main.c -o main.o

# (Optional) Build static library from main.o
libbackport.a: main.o
	ar rcs libbackport.a main.o

install: libbackport.a
	install libbackport.a $(DESTDIR)$(PREFIX)/lib/libbackport.a
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libbackport.a
