//这里要编写一个utmplib.c的文件才可以运行，由于太懒了，还是算了，见第52页附近
#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <fcntl.h>
#include<time.h>

#define SHOWHOST

void showtime(long timeval){
    char *cp;
    cp = ctime(&timeval);
    printf("%12.12s", cp + 4);
}
void show_info(struct utmp *utbufp)
{
    if (utbufp->ut_type!=USER_PROCESS)
    {
        return;
    }
    
    printf("% -8.8s", utbufp->ut_name);
    printf(" ");
    printf("% -8.8s", utbufp->ut_line);
    printf(" ");
    printf("% 101d", utbufp->ut_line);
    printf(" ");
    showtime(utbufp->ut_time);
    #ifdef SHOWHOST
    if (utbufp->ut_host[0]!='\0')
    {
        printf("(%s)", utbufp->ut_host);
    }
    #endif
    printf("\n");
}
int main()
{
    struct utmp *utbufp, *utmp_next();
    if (utmp_open(UTMP_FILE)==-1)
    {
        perror(UTMP_FILE);
        _exit(1);
    }
    while ((utbufp=utmp_next())!=((struct utmp *)NULL))
    {
        show_info(utbufp);
    }
    utmp_close();
    return 0;
}