#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BUFFER_LENGTH 1024              

static char receive[BUFFER_LENGTH];     

int main(){
    int ret, fd;
    char text[BUFFER_LENGTH];

    fd = open("/dev/hu_device", O_RDWR); 
    if (fd < 0){
       printf("%s%s", KRED,  "GG, can not open the device !");
       return errno;
    }
    printf("%s%s\n", KMAG, "input a line of nums");

    // 讲数据的写入其中的
    scanf("%s", text);                
    printf("%s%s\n",KBLU, text);
    if((ret = write(fd, text, strlen(text) + 1)) < 0){
        printf("%s%s\n", KRED,  "GG, write data failed!");
    }


    printf("%s%s\n", KGRN, "the second num");

    scanf("%s", text);                
    printf("%s%s\n",KBLU, text);
    if((ret = write(fd, text, strlen(text) + 1)) < 0){
        printf("%s%s\n", KRED,  "GG, write data failed!");
    }

    printf("%s%s\n", KGRN, "press any char to get res");

    getchar();

    if( (ret = read(fd, receive, BUFFER_LENGTH)) < 0){
        printf("%s%s\n", KRED,  "GG, read data failed!");
    } 
    printf("vector adder has res : %s\n", receive);
    return 0;
}