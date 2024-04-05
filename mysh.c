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
        //if(token != NULL){
        else{
           // if(strcmp(token, "<") != 0 && strcmp(token, ">") != 0 && strcmp(token, "|") != 0 && 
           //     strstr(token, "*") == NULL){
            args[argcount] = malloc(strlen(token) + 1); // +1 for null terminator
            if (!args[argcount]) {
                perror("malloc");
                // free previously allocated memory here
                return -1;
            }
            strcpy(args[argcount], token);
            argcount++;
            token = strtok(NULL, " \n");
        //        }
            
        }

        
    }
    args[argcount] = NULL; // null-termiate the argument list

    pid_t child = fork();
    
    if(child==0){
        if(p1in == 1){ //redirect standard input
            int fd = open(prog1in, O_RDONLY);
            dup2(fd, 0);
        }
        if(p1out==1){ //redirect standard output
            int fd = open(prog1out, O_WRONLY|O_TRUNC|O_CREAT, 0640);
            dup2(fd, 1);
        }
        execv(prog1, args);
        perror("execv"); // this only reache if execv fails
        exit(EXIT_FAILURE); // ensures child process exits if execv fails
    }
    else if (child == -1){
        perror("fork");
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

int readInput(FILE *input, int fd) {
    char *buf = malloc(sizeof(char) * 50); // Allocate once outside the loop
    if (buf == NULL) {
        perror("malloc");
        return -1;
    }
    if (isatty(fd) == 1) {
        printf("Enter a command:\n");
    }
    while(fgets(buf, 50, input) != NULL){
        if(buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t' && isatty(fd) 
            && (buf[4]==' ' || buf[4]=='\n')){
            printf("Exiting\n");
            break;
        }
        executeCmd(buf);
        if (isatty(fd) == 1) {
            printf("Enter a command:\n");
        }
    }
    free(buf); // Free once after the loop ends
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
