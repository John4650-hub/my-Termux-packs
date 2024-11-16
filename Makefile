TERMUX_PREFIX := /data/data/com.termux/files/usr
TERMUX_BASE_DIR := /data/data/com.termux/files
CFLAGS += -Wall -Wextra -Werror -Oz

liblisdir.so: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -DTERMUX_PREFIX=\"$(TERMUX_PREFIX)\" -DTERMUX_BASE_DIR=\"$(TERMUX_BASE_DIR)\" -shared -fPIC -o liblisdir.so
