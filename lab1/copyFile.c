#include <stdio.h>
#include <unistd.h>
#include <string.h>
/**
 * 实现的 mv 的部分功能
 * 1. 之前含有文件， 查询
 * 2. 对于路径判断 
 * 3. 实现os 的兼容
 */

#define maxn 10000

char pool[maxn]; 

int main(int argc, char *argv[]) { 
    if(argc < 3){
        printf("Only source and destination are given\n");
        return 0;
    }

    FILE * fs = fopen(argv[1], "rb");
    FILE * ft = fopen(argv[2], "wb");
    
    if(fs == NULL) {
        printf("can not open source file");
        return 0;
    }

    if(ft == NULL) {
        printf("can not open destination file");
        return 0;
    }
    
    
    int size;
    while((size = fread(pool, sizeof(char), 1000 , fs)) != 0){
        printf("%d", size);
        fwrite(pool, sizeof(char), size, ft);
    }

    printf("over");
    return 0;
}