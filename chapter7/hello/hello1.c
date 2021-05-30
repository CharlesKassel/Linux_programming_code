#include <curses.h>
#include <stdio.h>

main()
{
    initscr(); /*turn on curses*/
               /*send requests*/
    clear();
    move(10, 20);
    addstr("Hello world");
    move(LINES - 1, 0);
    refresh(); //update the screen
    getch();   //wait for user input
    endwin();
}