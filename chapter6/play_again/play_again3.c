#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#define ASK "Do you want another transaction"
#define TRIES 3
#define SLEEPTIME 2
#define BEEP putchar('\a')

get_response(char *, int);
get_ok_char();
set_cr_noecho_mode();
set_nodelay_mode();
tty_mode(int);

main()
{
    int response;
    tty_mode(0);
    set_cr_noecho_mode();
    set_nodelay_mode();
    response = get_response(ASK, TRIES);
    tty_mode(1);
    return response;
}

get_response(char *question, int maxtries)
{
    int input;
    printf("%s(y/n)?", question);
    fflush(stdout);
    while (1)
    {
        sleep(SLEEPTIME);
        input = tolower(get_ok_char());
        if (input == 'y')
            return 0;
        if (input == 'n')
            return 1;
        if (maxtries-- == 0)
            return 2;
        BEEP;
    }
}

get_ok_char()
{
    int c;
    while ((c = getchar()) != EOF && strchr("yYnN", c) == NULL)
        ;
    return c;
}

set_cr_noecho_mode()
{
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttystate);
}

set_nodelay_mode()
{
    int termflags;
    termflags = fcntl(0, F_GETFL);
    termflags |= O_NDELAY;
    fcntl(0, F_SETFL, termflags);
}

tty_mode(int how)
{
    static struct termios original_mode;
    static int original_flags;
    if (how == 0)
    {
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
    }
    else
    {
        tcsetattr(0, TCSANOW, &original_mode);
        fcntl(0, F_SETFL, original_flags);
    }
}