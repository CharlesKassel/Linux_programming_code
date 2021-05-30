#include <curses.h>
#include <stdio.h>

main()
{
    int i;
    initscr();
    clear();
    for (i = 0; i < LINES; ++i)
    {
        move(i, i + i);
        if (i % 2 == 1)
            standout();
        addstr("Hello world");
        if (i % 2 == 1)
            standend();
        refresh();
        sleep(0.5);
        move(i, i + i);
        addstr("                    ");
    }
    endwin();
}