#include <signal.h>
#include <stdio.h>

void wakeup(int);

main()
{
    void wakeup(int);
    void apointment1(int);

    printf("about to sleep for 4 seconds \n");
    signal(SIGALRM, wakeup);
    signal(SIGINT, apointment1);
    alarm(4);
    pause();
    printf("Morning so soon? \n");
}

void apointment1(int signum)
{
    printf("Test of Crtl-C\n");
}
void wakeup(int signum)
{
#ifndef SHHHH
    printf("Alarm received from kernel\n");
#endif
}