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
    char *buf = malloc(sizeof(char)*50);
    if(isatty(fd) == 1){
        printf("Enter a command:\n");
    }
    while(fgets(buf, 50, input) != NULL){
        if(buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t' && isatty(fd) 
            && (buf[4]==' ' || buf[4]=='\n')){
            printf("Exiting\n");
            break;
        }
        cond = executeCmd(buf);
        if(isatty(fd) == 1){
            printf("Enter a command:\n");
        }
        char *buf = malloc(sizeof(char)*50);
    }
    
    
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