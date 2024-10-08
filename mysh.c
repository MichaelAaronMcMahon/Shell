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
#include "lines.h"

int handleBuiltInCommands(char *cmd, char **args) {
    char* builtIns[] = {"cd", "pwd", "exit", "which", NULL};

    if (strcmp(cmd, "cd") == 0) {
        
        if (args[1] == NULL || args[2] != NULL) {
            // cd command requires exactly one argument (the directory path)
            printf("cd: incorrect number of arguments\n");
            return 1; // dislpay that the command was handled (even if there was an error)
        }

        if (chdir(args[1]) != 0) {
            // chdir has failed, print the error message
            perror("cd");
            return 1; // comand was handled
        }
        return 1; // command successfully executed
        // Implementation omitted for brevity
        
    } else if (strcmp(cmd, "pwd") == 0) {

        if (args[1] != NULL) {
            // pwd command does not take any arguments
            printf("pwd: too many arguments\n");
            return 1; // indicate that the command was handled (even if there was an error)
        }

        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd); // print the current working directory
        } else {
            // getcwd has  failed, print the error message
            perror("pwd");
        }
        return 1; // command successfully executed (or attempted)
        
        // Implementation omitted for brevity
    } else if (strcmp(cmd, "exit") == 0) {

        // print any arguments received with the exit command
        if (args[1] != NULL) {
            printf("Exiting: ");
            for (int i = 1; args[i] != NULL; i++) {
                printf("%s ", args[i]);
            }
            printf("\n");
        } else {
            printf("Exiting.\n");
        }
        exit(EXIT_SUCCESS); // EXIT_SUCCESS shows successful termination
    
    
        // Implementation omitted for brevity
    } else if (strcmp(cmd, "which") == 0) {
        if (args[1] == NULL || args[2] != NULL) {
            printf("which: incorrect number of arguments\n");
            return 1;
        }

        for (int i = 0; builtIns[i] != NULL; i++) {
            if (strcmp(args[1], builtIns[i]) == 0) {
                printf("%s: is a shell built-in\n", args[1]);
                return 1;
            }
        }

        char* pathEnv = getenv("PATH");
        if (pathEnv == NULL) {
            printf("Error: PATH environment variable not found.\n");
            return 1;
        }

        char* pathEnvCopy = strdup(pathEnv); // Use strdup to create a modifiable copy
        if (pathEnvCopy == NULL) {
            perror("Error duplicating PATH");
            return 1;
        }

        char* fullPath = malloc(2048); // Allocate memory for fullPath
        if (fullPath == NULL) {
            perror("Malloc failed");
            free(pathEnvCopy); // Cleanup
            return 1;
        }

        int found = 0;
        char* path = strtok(pathEnvCopy, ":");
        struct stat statbuf;
        
        while (path != NULL && !found) {
            snprintf(fullPath, 2048, "%s/%s", path, args[1]);
            if (stat(fullPath, &statbuf) == 0 && (statbuf.st_mode & S_IXUSR || statbuf.st_mode & S_IXGRP || statbuf.st_mode & S_IXOTH)) {
                printf("%s\n", fullPath);
                found = 1; // Exit the loop when found
            }
            path = strtok(NULL, ":");
        }

        if (!found) {
            printf("%s: not TT found\n", args[1]);
        }

        free(pathEnvCopy); // Free the copied PATH string
        free(fullPath); // Free the allocated fullPath
        return 1;
    }
    return 0; // Not a built-in command
}

int lastCmd[2] = {0,0}; //exit statuses of the two processes are stored here to be referenced in the next command
char* ulb = "/usr/local/bin/";
int p1mal=0;

int executeCmd(void *st, char* buf, int fd){
    char* prog1in; //names of redirected input and output files
    char* prog1out;
    char* prog2in;
    char* prog2out;
    int p1in = 0;
    int p1out = 0; 
    int p2in = 0;
    int p2out = 0;
    int piping = 0;
    int pipeIndex = 0;
    char* token = strtok(buf, " \n");
    char* prevFail = "Previous command failed, command not executed.";
    char* prevSuccess = "Previous command succeeded, command not executed.";
    if(token[0]=='e' && token[1]=='x' && token[2]=='i' && token[3]=='t' && isatty(fd)){
        return 0; //temporary until built in commands are done
    }
    if(strcmp(token, "then") == 0){ //checks for then conditional
        if(lastCmd[0] != 1 && lastCmd[1] != 1){ //checks if the previous command failed
            token = strtok(NULL, " \n"); //proceeds with command if it didn't
        }
        else{
            printf("%s\n", prevFail);
            if (isatty(fd)) printf("Enter a command: \n");
            return -1; //exits executeCmd if it did
        }
    }
    if(strcmp(token, "else") == 0){ //checks for else conditional
        if(lastCmd[0] == 1 || lastCmd[1] == 1){ //checks if the previous command failed
            token = strtok(NULL, " \n"); //proceeds if it did
        }
        else{
            printf("%s\n", prevSuccess);
            if (isatty(fd)) printf("Enter a command: \n");
            return -1; //exits if it didn't
        }
    }
    char* prog1 = malloc(strlen(token) + 1); // +1 for null terminator
    if (!prog1) {
        perror("malloc");
        return -1;
    }
    strcpy(prog1, token);
    char *args[15]; // increases size directly, consider dynamic resizing if needed
    char *args1[15];
    args[0] = prog1; // first argument should be the command itself
    int argcount = 1; // start from 1 to account for the command itself
    int currentProg = 1; //will be changed to 2 when a pipe is found
    token = strtok(NULL, " \n");
    while(token != NULL && argcount < 14){ // leave space for NULL terminator
        if(strstr(token, "*")){ //checks for * wildcard
            char* wilddir = malloc(strlen(token)+1);
            strcpy(wilddir, token);
            int maxSlash = 0;
            for(int i=0; i<strlen(wilddir); i++){
                if (wilddir[i] == '/') maxSlash = i;
            } //wilddir is the full argument
            char* wildfile = malloc(strlen(wilddir) - maxSlash + 1);
            int wildindex = 0;
            strcpy(wildfile, wilddir+maxSlash+1); //copies file name to wildfile
            for(int i=0; i<strlen(wildfile); i++){
                if (wildfile[i]=='*'){
                    wildindex = i;
                    break;
                }
            }
            DIR *handle;
            if(maxSlash != 0){
                wilddir[maxSlash] = '\0'; //terminates wilddir at the last slash, making it just the subdirectories
                handle = opendir(wilddir); //opens directory
            }
            else{
                handle = opendir(".");
            }
            int finds = 0;
            struct dirent *sd = readdir(handle);
            while(sd != NULL){ //iterates trough each directory entry in wilddir
                int match = 0;
                wildfile[wildindex] = '\0';
                char* argname = malloc(strlen(sd->d_name) + 1);
                strcpy(argname, sd->d_name);
                char argorig = argname[wildindex];
                if(strlen(argname)+1 >= wildindex) argname[wildindex] = '\0';
                if(strcmp(argname, wildfile) == 0) match++; //compares post-wildcard parts
                argname[wildindex] = argorig;
                wildfile[wildindex] = '*';
                if(strcmp(wildfile+wildindex+1, argname+(strlen(argname) - wildindex)) == 0) match++; //compares pre-wildcard parts
                if(match==2){ //add matching file to arglist
                    args[argcount] = malloc(strlen(argname) + 1); // +1 for null terminator
                    if (!args[argcount]) {
                        perror("malloc");
                        // free previously allocated memory here
                        //return -1;
                    }
                    strcpy(args[argcount], argname);
                    argcount++;
                    finds++;
                }
                
                sd = readdir(handle); //iterate to next file in wilddir
            }
            if(finds == 0) {
                args[argcount] = malloc(strlen(token) + 1); // +1 for null terminator
                if (!args[argcount]) {
                    perror("malloc");
                    // free previously allocated memory here
                    //return -1;
                }
                strcpy(args[argcount], token);
                argcount++;
            }
            token = strtok(NULL, " \n");
        }
        else if(strcmp(token, ">") == 0){ //checks for output redirection
            token = strtok(NULL, " \n");
            if(currentProg == 1){
                prog1out = malloc(sizeof(char)*strlen(token) + 1);
                strcpy(prog1out, token);
                p1out = 1;
            }
            else{
                prog2out = malloc(sizeof(char)*strlen(token) + 1);
                strcpy(prog2out, token);
                p2out = 1;
            }
            token = strtok(NULL, " \n");
        }
        else if(strcmp(token, "<") == 0){ //checks for input redirection
            token = strtok(NULL, " \n");
            if(currentProg == 1){
                prog1in = malloc(sizeof(char)*strlen(token) + 1);
                strcpy(prog1in, token);
                p1in = 1;
            }
            else{
                prog2in = malloc(sizeof(char)*strlen(token) + 1);
                strcpy(prog2in, token);
                p2in = 1;
            }
            token = strtok(NULL, " \n");
        }
        else if (strcmp(token, "|") == 0){ //checks for piping
            piping = 1;
            args[argcount] = NULL; //null terminates args for prog1
            pipeIndex = argcount+1; //sets index of the beginning of args for prog2
            argcount++;
            
            token = strtok(NULL, " \n");
            if(token != NULL){
                char *prog2 = malloc(strlen(token) +1);
                strcpy(prog2, token);
                args[argcount] = prog2; //begins setting the args for prog2
                argcount++;
                currentProg = 2; //sets currentProg to prog2
                token = strtok(NULL, " \n");
            }
            
        }
        else{
            args[argcount] = malloc(strlen(token) + 1); // +1 for null terminator
            if (!args[argcount]) {
                perror("malloc");
                // free previously allocated memory here
                //return -1;
            }
            strcpy(args[argcount], token);
            argcount++;
            token = strtok(NULL, " \n");
        }
    }
    args[argcount] = NULL; // null-termiate the argument list
    if(strcmp(args[0], "cd") == 0){
        token = strtok(NULL, " \n");
        if(argcount > 2){
            printf("chdir: too many arguments\n");
            printf("Enter a command: \n");
            return 1;
        }
        else{
            int cdstat = chdir(args[1]);
            if(cdstat == -1) printf("chdir error");
            printf("Enter a command: \n");
            return 1;
        }
    }
    int p[2];
    pipe(p); //sets up pipe
    
    if(fork() == 0){
        if(handleBuiltInCommands(args[0], args)){
            exit(0);      
        }
        if(piping==1){
            close(p[0]);
            dup2(p[1], 1); //sets prog1's output to the write end of the pipe
        }
        if(p1in == 1){ //redirect standard input
            int fd = open(prog1in, O_RDONLY);
            dup2(fd, 0);
        }
        if(p1out==1){ //redirect standard output
            int fd = open(prog1out, O_WRONLY|O_TRUNC|O_CREAT, 0640);
            dup2(fd, 1);
        }
        if(strstr(args[0], "/") == NULL){
            printf("\n");
            char* p1 = malloc(15+strlen(args[0]) + 1);
            strcat(p1, ulb);
            strcat(p1, args[0]);
            char *p1fix = p1 + (strlen(p1) - 15 - strlen(args[0]));
            if(access(p1fix, F_OK) == 0){
                args[0] = p1fix;
                execv(p1fix, args);
            }
            
            char* p2 = malloc(9+strlen(args[0]) + 1);
            strcat(p2, "/usr/bin/");
            strcat(p2, args[0]);
            
            //printf("p2: %s\n", p2);
            char *p2fix = p2 + (strlen(p2) - 9 - strlen(args[0]));
            
            //printf("p2fix: %s\n", p2fix);
            if(access(p2fix, F_OK) == 0){
                args[0] = p2fix;
                execv(p2fix, args);
            }
            //free(p2);
            char* p3 = malloc(5+strlen(args[0]) + 1);
            strcat(p3, "/bin/");
            strcat(p3, args[0]);
            char *p3fix = p3 + (strlen(p3) - 5 - strlen(args[0]));
            
            if(access(p3fix, F_OK) == 0){
                args[0] = p3fix;
                execv(p3fix, args);
            }
            //free(p3);
        }
        else{
            //printf("args[0]: %s\n", args[0]);
            execv(args[0], args); //executes prog1 with args array
        }
        
        execv(args[0], args); //executes prog1 with args array 
        perror("execv"); // this only reache if execv fails
        exit(EXIT_FAILURE); // ensures child process exits if execv fails
    }
    //else{
    //    perror("fork");
   // }
    if(piping==1){
        if(fork() == 0){
            //printf("p1mal: %d\n", p1mal);
            close(p[1]);
            dup2(p[0], 0); //sets input to read end of pipe
            if(p2in == 1){ //redirect standard input
                int fd = open(prog2in, O_RDONLY);
                dup2(fd, 0);
            }
            if(p2out==1){ //redirect standard output
                int fd = open(prog2out, O_WRONLY|O_TRUNC|O_CREAT, 0640);
                dup2(fd, 1);
            }
            if(strstr(args[pipeIndex], "/") == NULL && strcmp(args[pipeIndex],"cd") != 0 && strcmp(args[pipeIndex],"which") != 0 &&
            strcmp(args[pipeIndex],"pwd") != 0 && strcmp(args[pipeIndex],"exit") != 0){
                //printf("%s\n", args[pipeIndex]);
                char* p1_2 = malloc(15+strlen(args[pipeIndex]) + 1);
                strcat(p1_2, "/usr/local/bin/");
                strcat(p1_2, args[pipeIndex]);
                if(access(p1_2, F_OK) == 0){
                    execv(p1_2, args+pipeIndex);
                }
                char* p2_2 = malloc(9+strlen(args[pipeIndex]) + 1);
                strcat(p2_2, "/usr/bin/");
                strcat(p2_2, args[pipeIndex]);
                if(access(p2_2, F_OK) == 0){
                    execv(p2_2, args+pipeIndex);
                }
                char* p3_2 = malloc(5+strlen(args[pipeIndex]) + 1);
                strcat(p3_2, "/bin/");
                strcat(p3_2, args[pipeIndex]);
                if(access(p3_2, F_OK) == 0){
                    execv(p3_2, args+pipeIndex);
                }
            }
            else{
                execv(args[pipeIndex], args+pipeIndex); //executes prog2 with latter section of args array
            }
            //execv(args[pipeIndex], args+pipeIndex);
            perror("execv"); // this only reache if execv fails
            exit(EXIT_FAILURE); // ensures child process exits if execv fails*/
        }
        //else if (child2 == -1){
        //    perror("fork");
       // }
    }
   // else if (child1 == -1){
   //     perror("fork");
   // }
    
    int wstatus1;
    int wstatus2;
    pid_t wpid1;
    pid_t wpid2;
    close(p[0]);
    close(p[1]);
    wait(&wstatus1);
    wait(&wstatus2);
    for(int i = 1; i < argcount; i++){ // start from 1, prog1 is args[0]
        if(args[i] != NULL) free(args[i]);
    }
    free(prog1);
    lastCmd[0] = WEXITSTATUS(wstatus1);
    lastCmd[1] = WEXITSTATUS(wstatus2);
    if(isatty(fd) == 1){
        printf("Enter a command: \n");
    }
}

int readInput(FILE *input, int fd) {
    int n=0;
    read_lines(fd, executeCmd, &n);
    
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
        printf("Enter a command: \n");
    }
    readInput(input, fd);
    return 0;

}
