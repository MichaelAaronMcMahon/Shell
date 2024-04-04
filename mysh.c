#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

int executeCmd(char* buf){
    char* token = strtok(buf, " \n");
    int wstatus;
    char * const* args = {NULL};
    pid_t child = fork();
    if(strcmp(token, "quit") == 0){
        printf("match\n");
        return 2;
    }
    if(child==0){
        //printf("token: %s\n", token);
        int st = execv(token, args);
        //printf("%d\n", st);
        printf("shouldn't be here");
    }
    else if (child == -1){
        printf("err\n");
    }
    wait(&wstatus);
    
    /*while(token != NULL){
        

        token = strtok(NULL, "\n");
    }*/
    
}

int readInput(FILE *input, int fd){
    int cond = 1;
    while(cond==1){
        if(isatty(fd) == 1){
        printf("Enter a command:\n");
        }
        char *buf = malloc(sizeof(char)*50);
        fgets(buf, 50, input);
        cond = executeCmd(buf);
        printf("%d\n", cond);
    }
    
    
    //printf("%s\n", buf);
    
}

int main(int argc, char ** argv){
    int fd;
    FILE *input;
    if(argc == 1){
        //interactive
        fd = 0;
        input = stdin;
    }
    else{
        //batch
        fd = open(argv[1], O_RDONLY);
        input = fopen(argv[1], "r");
    }
    if(isatty(fd) == 1){
        printf("Welcome to My Shell\n");
    }
    readInput(input, fd);
    
    


}