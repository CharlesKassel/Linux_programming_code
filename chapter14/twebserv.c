#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

time_t server_started;
int server_bytes_sent;
int server_requests;

main(int ac, char *av[])
{
    int sock, fd;
    int *fdptr;
    pthread_t worker;
    pthread_attr_t attr;

    void *handle_call(void *);

    if (ac == 1)
    {
        fprintf(stderr, "usage: tws portnum\n");
        exit(1);
    }
    sock = make_server_socket(atoi(av[1]));
    if (sock == -1)
    {
        perror("making socket");
        exit(2);
    }

    setup(&attr);

    while (1)
    {
        fd = accept(sock, NULL, NULL);
        ++server_requests;
        fdptr = malloc(sizeof(int));
        *fdptr = fd;
        pthread_create(&worker, &attr, handle_call, fdptr);
    }
}

setup(pthread_attr_t *attrp)
{
    pthread_attr_init(attrp);
    pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED);

    time(&server_started);
    server_requests = 0;
    server_bytes_sent = 0;
}

void *handle_call(void *fdptr)
{
    FILE *fpin;
    char request[BUFSIZ];
    int fd;

    fd = *(int *)fdptr;
    free(fdptr);

    fpin = fdopen(fd, "r");
    fgets(request, BUFSIZ, fpin);
    printf("got a call on %d: request = %s", fd, request);
    skip_rest_of_header(fpin);

    process_rq(request, fd);

    fclose(fpin);
}

skip_rest_of_header(FILE *fp)
{
    char buf[BUFSIZ];
    while (fgets(buf, BUFSIZ, fp) != NULL && strcmp(buf, "\r\n") != 0)
        ;
}

process_rq(char *rq, int fd)
{
    char cmd[BUFSIZ], arg[BUFSIZ];

    if (sscanf(rq, "%s%s", cmd, arg) != 2)
        return;
    sanitize(arg);
    printf("sanitized version is %s\n", arg);

    if (strcmp(cmd, "GET") != 0)
        not_implemented(fd);
    else if (built_in(arg, fd))
        ;
    else if (not_exist(arg))
        do_404(arg, fd);
    else if (isadir(arg))
        do_ls(arg, fd);
}

sanitize(char *str)
{
    char *src, *dest;

    src = dest = str;

    while (*src)
    {
        if (strncmp(src, "/../", 4) == 0)
            src += 3;
        else if (strncmp(src, "//", 2) == 0)
            src++;
        else
            *dest++ = *src++;
    }
    *dest = '\0';
    if (*str == '/')
        strcpy(str, str + 1);

    if (str[0] == '\0' || strcmp(str, "./") == 0 || strcmp(str, "./..") == 0)
        strcpy(str, ".");
}

built_in(char *arg, int fd)
{
    FILE *fp;

    if (strcmp(arg, "status") != 0)
        return 0;
    http_reply(fd, &fp, 200, "OK", "text/plain", NULL);

    fprintf(fp, "Server started: %s", ctime(&server_started));
    fprintf(fp, "Tota; requests: %d\n", server_requests);
    fprintf(fp, "Bytes sent out: %d\n", server_bytes_sent);
    fclose(fp);
    return 1;
}

http_reply(int fd, FILE **fpp, int code, char *msg, char *type, char *content)
{
    FILE *fp = fdopen(fd, "w");
    int bytes = 0;

    if (fp != NULL)
    {
        bytes = fprintf(fp, "HTTP/1.0 %d %s\r\n", code, msg);
        bytes += fprintf(fp, "Content-type: %s\r\n\r\n", type);
        if (content)
            bytes += fprintf(fp, "%s\r\n", content);
    }
    fflush(fp);
    if (fpp)
        *fpp = fp;
    else
        fclose(fp);
    return bytes;
}

not_implemented(int fd)
{
    http_reply(fd, NULL, 501, "Not Implemented", "text/plain", "That command is not implemented");
}

do_404(char *item, int fd)
{
    http_reply(fd, NULL, 404, "Not Found", "text/plain", "The item you seek is not here");
}

isadir(char *f)
{
    struct stat info;
    return (stat(f, &info) != -1 && S_ISDIR(info.st_mode));
}

not_exist(char *f)
{
    struct stat info;
    return (stat(f, &info) == -1);
}

do_ls(char *dir, int fd)
{
    DIR *dirptr;
    struct dirent *direntp;
    FILE *fp;
    int bytes = 0;

    bytes = http_reply(fd, &fp, 200, "OK", "text/plain", NULL);
    bytes += fprintf(fp, "Listing of Directory %s\n", dir);

    if ((dirptr = opendir(dir)) != NULL)
    {
        while (direntp = readdir(dirptr))
        {
            bytes += fprintf(fp, "%s\n", direntp->d_name);
        }
        closedir(dirptr);
    }
    fclose(fp);
    server_bytes_sent += bytes;
}

char *file_type(char *f)
{
    char *cp;
    if ((cp = strrchr(f, '.')) != NULL)
        return cp + 1;
    return "-";
}

do_cat(char *f, int fd)
{
    char *extension = file_type(f);
    char *type = "text/plain";
    FILE *fpsock, *fpfile;
    int c;
    int bytes = 0;

    if (strcmp(extension, "html") == 0)
        type = "text/html";
    else if (strcmp(extension, "gif") == 0)
        type = "image/gif";
    else if (strcmp(extension, "jpg") == 0)
        type = "image/jpeg";
    else if (strcmp(extension, "jpeg") == 0)
        type = "image/jpeg";

    fpsock = fdopen(fd, "w");
    fpfile = fopen(f, "r");
    if (fpsock != NULL && fpfile != NULL)
    {
        bytes = http_reply(fd, &fpsock, 200, "OK", type, NULL);
        while ((c = getc(fpfile)) != EOF)
        {
            putc(c, fpsock);
            ++bytes;
        }
        fclose(fpfile);
        fclose(fpsock);
    }
    server_bytes_sent += bytes;
}