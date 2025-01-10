#include <ncurses.h>
#include <md4c.h>
#include <string>
#include <iostream>

// Callback function to handle parsed markdown events
static void md_callback(const MD_CHAR* text, MD_SIZE size, void* userdata) {
	 // Render the text using ncurses
	 printw("%.*s", (int)size, text);
}

int main() {
	 // Initialize ncurses
	 initscr();
	 cbreak();
	 noecho();

	 // Example markdown text
	 const char* markdown = "# Hello, ncurses!\nThis is *markdown* in a TUI.";

	 // Parse the markdown
	 md_parse(markdown, strlen(markdown), md_callback, nullptr, MD_DIALECT_COMMONMARK, 0);

	 // Refresh and wait for user input
	 refresh();
	 getch();

	 // End ncurses mode
	 endwin();

	 return 0;
}
