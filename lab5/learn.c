#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "./header/config.h"

int main(void){
    char * args[] = {"vim", VIM_BUF_PATH, NULL};
    int status;
    if(!fork()){
        execvp("vim", args);
    }
    wait(&status);

}

// #include <stdio.h>
// #include <stdlib.h>
