#include <curses.h>
#include <stdio.h>

main()
{
    int i;

    initscr();
    clear();
    for (i = 0; i < LINES; ++i)
    {
        move(i, i + 1);
        if (i % 2 == 1)
        {
            standout();
        }
        addstr("Hello world");
        if (i % 2 == 1)
        {
            standend();
        }
    }
    refresh();
    getch();
    endwin();
}