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

int handleBuiltInCommands(char *cmd, char **args) {
    if (strcmp(cmd, "cd") == 0) {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
        return 1;
    } else if (strcmp(cmd, "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    } else if (strcmp(cmd, "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else if (strcmp(cmd, "which") == 0) {
        // which command implementation
        return 1;
    }
    return 0; // Not a built-in command
}

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

int executeCmd(char* buf){
    char* token = strtok(buf, " \n");
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
    while(token != NULL && argcount < 14){ // leve space for NULL terminator
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
                        return -1;
                    }
                    strcpy(args[argcount], argname);
                    argcount++;
                    finds++;
                }
                
                sd = readdir(handle); //iterate to next file in wilddir
            }
            if(finds != 0) token = strtok(NULL, " \n");
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
                return -1;
            }
            strcpy(args[argcount], token);
            argcount++;
            token = strtok(NULL, " \n");
        }
    }
    args[argcount] = NULL; // null-termiate the argument list
    
    int p[2];
    pipe(p); //sets up pipe
    pid_t child1 = fork(); //process for prog1
    
    if(child1==0){
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
        execv(prog1, args); //executes prog1 with args array 
        perror("execv"); // this only reache if execv fails
        exit(EXIT_FAILURE); // ensures child process exits if execv fails
    }
    pid_t child2 = fork(); //process for prog2
    if(piping==1){
        if(child2 == 0){
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
            execv(args[pipeIndex], args+pipeIndex); //executes prog2 with latter section of args array
            perror("execv"); // this only reache if execv fails
            exit(EXIT_FAILURE); // ensures child process exits if execv fails
        }
        else if (child2 == -1){
            perror("fork");
        }
    }
    else if (child1 == -1){
        perror("fork");
    }
    close(p[0]);
    close(p[1]);
    int wstatus1;
    int wstatus2;
    wait(&wstatus1);
    wait(&wstatus2);

    // Free allocated memory
    for(int i = 1; i < argcount; i++){ // start from 1, prog1 is args[0]
        free(args[i]);
    }
    free(prog1);

    return WIFEXITED(wstatus1) ? WEXITSTATUS(wstatus1) : -1;
    return WIFEXITED(wstatus2) ? WEXITSTATUS(wstatus2) : -1;
}

int readInput(FILE *input, int fd) {
    char *buf = malloc(sizeof(char) * 256);
    if (buf == NULL) {
        perror("malloc");
        return -1;
    }

    if (isatty(fd) == 1) {
        printf("Enter a command:\n");
    }
    while (fgets(buf, 256, input) != NULL) {
        char *tmpBuf = strdup(buf); // Duplicate buf for tokenization without modifying the original buf
        if (!tmpBuf) {
            perror("strdup");
            break;
        }

        char *args[15];
        int argcount = 0;
        char *token = strtok(tmpBuf, " \n");

        while (token != NULL && argcount < 14) {
            args[argcount++] = token;
            token = strtok(NULL, " \n");
        }
        args[argcount] = NULL;

        if (argcount > 0 && !handleBuiltInCommands(args[0], args)) {
            executeCmd(buf); // Pass the original buffer directly
        }

        free(tmpBuf); // Free the duplicated buffer

        if (isatty(fd) == 1) {
            printf("Enter a command:\n");
        }
    }

    free(buf);
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
