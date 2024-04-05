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

int main(void) {
    char buf[30];
    while(fgets(buf, sizeof(buf), stdin)){
        printf("%s", buf);
    }
    printf("end of redir\n");
    return 0;
}