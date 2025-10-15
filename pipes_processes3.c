#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void die(const char *m){
    perror(m);
    _exit(1);
}

int main(int argc, char *argv[]){
    if(argc!=2){
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        return 1;
    }
    const char *pattern=argv[1];

    int p12[2], p23[2];
    if(pipe(p12)==-1) die("pipe p12");
    if(pipe(p23)==-1) die("pipe p23");

    pid_t pid_grep=fork();
    if(pid_grep<0) die("fork grep");

    if(pid_grep==0){
        pid_t pid_sort=fork();
        if(pid_sort<0) die("fork sort");

        if(pid_sort==0){
            if(dup2(p23[0], STDIN_FILENO)==-1) die("dup2 sort stdin");
            close(p23[0]); close(p23[1]);
            close(p12[0]); close(p12[1]);
            execlp("sort", "sort", (char*)NULL);
            die("exec sort");
        }

        if(dup2(p12[0], STDIN_FILENO)==-1) die("dup2 grep stdin");
        if(dup2(p23[1], STDOUT_FILENO)==-1) die("dup2 grep stdout");
        close(p12[0]); close(p12[1]);
        close(p23[0]); close(p23[1]);
        execlp("grep", "grep", pattern, (char*)NULL);
        die("exec grep");
    }

    if(dup2(p12[1], STDOUT_FILENO)==-1) die("dup2 cat stdout");
    close(p12[0]); close(p12[1]);
    close(p23[0]); close(p23[1]);
    execlp("cat", "cat", "scores", (char*)NULL);
    die("exec cat");
}
