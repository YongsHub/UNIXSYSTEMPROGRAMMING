#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSGSIZE 16
char *msg1 = "hello, world #1";
char *msg2 = "hello, world #2";
char *msg3 = "hello, world #3";

int main(){
    char inbuf[MSGSIZE];
    int p[2], j;
    pid_t pid;

    /* 파이프를 개방한다. */
    if (pipe(p) == -1)
    {
            perror ("pipe call");
            exit (1);
    }
    switch (pid = fork()){
    case -1:
        perror ("fork call");
        exit (2);
    case 0:
            /* 만일 자식이면 읽기 화일 기술자를 닫고, 파이프에 쓴다 */
            close (p[0]);
            write (p[1], msg1, MSGSIZE);
            write (p[1], msg2, MSGSIZE);
            write (p[1], msg3, MSGSIZE);
            break;
    default:
            /* 만일 부모이면 쓰기 화일 기술자를 닫고, 파이프로부터 읽는다 */
            close (p[1]);
            for (j = 0; j < 3; j++)
            {
                    read (p[0], inbuf, MSGSIZE);
                    printf ("%s\n", inbuf);
            }
            wait(NULL);
    }
    exit (0);
}
                          
                                