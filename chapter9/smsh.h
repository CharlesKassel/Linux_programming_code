#define YES 1
#define NO 0

char *next_cmd();
char **splitline(char *);
void freelist(char **);
void *emalloc(int);
void *erealloc(void *, int);
int execute(char **);
void fatal(char *, char *, int);