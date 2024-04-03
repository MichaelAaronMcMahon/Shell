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

void executeCmd(char* buf){
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

int readInput(){
    int bytes;
    char *buf = malloc(sizeof(char)*50);
    bytes = read(0, buf, 50);
    //printf("%s\n", buf);
    executeCmd(buf);
}

int main(int argc, char ** argv){
    if(argc == 1){
        //interactive
        int cont = 1;
        while(cont==1){
            cont = readInput();
            //printf("%d\n", cont);
        }
    }

}