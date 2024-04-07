#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    for (int i=0; i<argc; i++){
        printf("%c\n", argv[i][0]);
    }
    return 0;
}