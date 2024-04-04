#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

int main(int argc, char ** argv){
    for(int i=0; i<argc; i++){
        printf("argv[%d]: %s\n", i, argv[i]);
    }
}