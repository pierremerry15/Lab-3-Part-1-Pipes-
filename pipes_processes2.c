#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF 1024

static void trim(char *s){
    if(!s) return;
    size_t n=strlen(s);
    while(n && (s[n-1]=='\n' || s[n-1]=='\r')) s[--n]='\0';
}

static void die(const char *m){
    perror(m);
    _exit(1);
}

int main(void){
    int p12[2], p21[2];
    if(pipe(p12)==-1) die("pipe p12");
    if(pipe(p21)==-1) die("pipe p21");

    pid_t pid=fork();
    if(pid<0) die("fork");

    if(pid==0){
        close(p12[1]);
        close(p21[0]);

        char in1[BUF]={0};
        ssize_t n=read(p12[0], in1, sizeof(in1)-1);
        if(n<0) die("read P2");
        in1[n]='\0';

        char stage1[BUF];
        snprintf(stage1, sizeof(stage1), "%showard.edu", in1);

        printf("Other string is: howard.edu\n");
        printf("Input : %s\n", in1);
        printf("Output : %s\n", stage1);
        fflush(stdout);

        char second[BUF]={0};
        printf("Input : ");
        fflush(stdout);
        if(!fgets(second, sizeof(second), stdin)) second[0]='\0';
        trim(second);

        char stage2[BUF];
        snprintf(stage2, sizeof(stage2), "%s%s", stage1, second);

        size_t outlen=strnlen(stage2, sizeof(stage2))+1;
        if(write(p21[1], stage2, outlen)<0) die("write P2->P1");

        close(p12[0]);
        close(p21[1]);
        _exit(0);
    }else{
        close(p12[0]);
        close(p21[1]);

        char first[BUF]={0};
        printf("Input : ");
        fflush(stdout);
        if(!fgets(first, sizeof(first), stdin)) first[0]='\0';
        trim(first);

        size_t len=strnlen(first, sizeof(first))+1;
        if(write(p12[1], first, len)<0) die("write P1->P2");

        char from_p2[BUF]={0};
        ssize_t n=read(p21[0], from_p2, sizeof(from_p2)-1);
        if(n<0) die("read P1");
        from_p2[n]='\0';

        char final_out[BUF];
        snprintf(final_out, sizeof(final_out), "%sgobison.org", from_p2);
        printf("Output : %s\n", final_out);
        fflush(stdout);

        close(p12[1]);
        close(p21[0]);
        return 0;
    }
}
