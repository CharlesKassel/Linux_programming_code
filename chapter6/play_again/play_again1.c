#include <stdio.h>
#include <termios.h>

#define QUESTION "Do you want another transaction"

tty_mode(int how)
{
    static struct termios original_mode;
    {
        tcgetattr(0, &original_mode);
    }
    else
    {
        return tcsetattr(0, TCSANOW, &original_mode);
    }
}

int get_response(char *question)
{
    int input;
    printf("%s(y/n)?", question);
    while (1)
    {
        switch (input = getchar())
        {
        case 'y':
        case 'Y':
            return 0;
        case 'n':
        case 'N':
        case EOF:
            return 1;
        default:
            printf("\ncannot understand %c,", input);
            printf("Please type y or n \n");
        }
    }
}

set_crmode()
{
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttystate);
}

main()
{
    int response;
    tty_mode(0);
    set_crmode();
    response = get_response(QUESTION);
    tty_mode(1);
    return response;
}