#include <ncurses.h>
#include <iostream>

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Get the size of the window
    int height, width, start_y, start_x;
    height = 10;
    width = 40;
    start_y = (LINES - height) / 2;
    start_x = (COLS - width) / 2;

    // Create a new window
    WINDOW* win = newwin(height, width, start_y, start_x);
    box(win, 0, 0);

    // Display some text
    mvwprintw(win, 1, 1, "Hello, ncurses!");
    mvwprintw(win, 3, 1, "Press any key to continue...");

    // Refresh the window to show the changes
    wrefresh(win);

    // Wait for user input
    wgetch(win);

    // Cleanup and close ncurses
    delwin(win);
    endwin();

    return 0;
}
