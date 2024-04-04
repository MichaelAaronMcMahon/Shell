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
    char* prog1 = malloc(strlen(token) + 1); // +1 for null terminator
    if (!prog1) {
        perror("malloc");
        return -1;
    }
    strcpy(prog1, token);

    char *args[15]; // increases size directly, consider dynamic resizing if needed
    args[0] = prog1; // first argument should be the command itself
    int argcount = 1; // start from 1 to account for the command itself

    token = strtok(NULL, " \n");
    while(token != NULL && argcount < 14){ // leve space for NULL terminator
        args[argcount] = malloc(strlen(token) + 1); // +1 for null terminator
        if (!args[argcount]) {
            perror("malloc");
            // free previously allocated memory here
            return -1;
        }
        strcpy(args[argcount], token);
        argcount++
        token = strtok(NULL, " \n");
    }
    args[argcount] = NULL; // null-termiate the argument list

    pid_t child = fork();
    
    if(chld==0){
        execv(prog1, args);
        perror("execv"); // this only reache if execv fails
        exit(EXIT_FAILURE); // ensures child process exits if execv fails
    }
    else if (child == -1){
        perror("fork")
    }
    
    int wstatus;
    wait(&wstatus);

    // Free allocated memory
    for(int i = 1; i < argcount; i++){ // start from 1, prog1 is args[0]
        free(args[i]);
    }
    free(prog1);

    return WIFEXITED(wstatus) ? WEXITSTATUS(wstatus) : -1;
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
