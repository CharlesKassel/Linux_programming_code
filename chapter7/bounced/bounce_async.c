#include <curses.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#define MESSAGE "hello"
#define BLANK "       "

int row = 10;
int col = 10;
int dir = 10;
int delay = 200;
int done = 0;

main()
{
    void on_alarm(int);
    void on_input(int);
    void enable_kbd_signals();

    initscr();
    crmode();
    noecho();
    clear();

    signal(SIGIO, on_input);   //在接收到信号SIGIO的时候调用on_input()函数
    enable_kbd_signals();      //打开接受键盘的信号
    signal(SIGALRM, on_alarm); //在接收到信号SIGALRM的时候调用alarm()函数
    set_ticker(delay);

    move(row, col);
    addstr(MESSAGE);

    while (!done)
        pause();
    endwin();
}

void on_input(int signum)
{
    int c = getch();

    if (c == 'Q' || c == EOF)
        done = 1;
    else if (c == ' ')
        dir = -dir;
}

void on_alarm(int signum)
{
    signal(SIGALRM, on_alarm);
    mvaddstr(row, col, BLANK);
    col += dir;
    mvaddstr(row, col, MESSAGE);
    refresh();

    if (dir == -1 && col <= 0)
        dir = 1;
    else if (dir == 1 && col + strlen(MESSAGE) >= COLS)
        dir = -1;
}

void enable_kbd_signals()
{
    int fd_flags;

    fcntl(0, F_SETOWN, getpid());
    fd_flags = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, (fd_flags | O_ASYNC));
}