#include <stdio.h>
#include<unistd.h>
#include <termios.h>
#define oops(s, x) \
    {              \
        perror(s); \
        _exit(x);  \
    }

main(int ac, char *av[])
{
    struct termios info;
    if (ac == 1)
    {
        _exit(0);
    }
    if (tcgetattr(0, &info) == -1)
    {
        oops("tcgettattr", 1);
    }
    if (av[1][0] == 'y')
    {
        info.c_lflag |= ECHO;
    }
    else
    {
        info.c_lflag &= ~ECHO;
    }
    if (tcsetattr(0, TCSANOW, &info) == -1)
    {
        oops("tcsetattr", 2);
    }
}
