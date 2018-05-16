#include "../header/config.h"
#include "../header/tools.h"
#include "../header/debug.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>


void load_inode(fFile fp, Integer inode_num){
    fseek(fptr, fs_config.BLOCK_SIZE + inode_num * sizeof(INODE), SEEK_SET);
    fread(fp, sizeof(INODE), 1, fptr);
}

void save_inode(fFile fp){
    unsigned int inode_num = fp->inode_num;
    fseek(fptr, fs_config.BLOCK_SIZE + inode_num * sizeof(INODE), SEEK_SET);
    fwrite(fp, sizeof(INODE), 1, fptr);
}


void inode_cons(fFile f, enum file_type t){
    // 在创建文件的使用的， 都是采用默认设置
    
    if(t == IS_DIR) 
        f->permission = 0x003c;
    else 
        f->permission = 0x001c; // 只有user 

    f->u_id = 0;
    f->g_id = 0;
    time(&f->st_mtime);

    f->len = 0;
    f->block_num = 0;
    f->ptr_pos = 0;

    memset(f->i_addr, 0, sizeof(f->i_addr));

    f->next_free = 0;
    f->parent = -2; // 默认初始化中间
    // parent 在 new unix file 中间书写
}

void seek(Integer block){
    if(block >= fs_config.BLOCK_NUM) assert(0);
    fseek(fptr, block * fs_config.BLOCK_SIZE, SEEK_SET);
}


void file_seek(fFile file, F_POS p){
    if(p == START){
        file->ptr_pos = 0;
    }else if(p == END){
        file->ptr_pos = file->len;
    }
}

void cd_inode(unsigned int inode_num, const char * file_name){
    // 添加分隔符 添加文件名称
    strcpy(CUR_PATH.path + strlen(CUR_PATH.path), "/");
    strcpy(CUR_PATH.path + strlen(CUR_PATH.path), file_name);
    CUR_PATH.inodes[CUR_PATH.p_inodes] = inode_num;
    CUR_PATH.p_inodes ++;
}

void cd_root(){
    CUR_PATH.p_inodes = 0;
    CUR_PATH.path[0] = '\0';
    if(DEBUG_FS) print_cur_path();

}

void cd_out(){
    // 将最后的一个 / 转化成为string 结束的符号
    int i = strlen(CUR_PATH.path);
    while(CUR_PATH.path[i] != '/') i --;
    CUR_PATH.path[i] = '\0';
    CUR_PATH.p_inodes --;
}

void print_file_msg(Integer inode_num){
    printf("%s", KBLU);
    // get inode
    fFile f = (fFile)malloc(sizeof(INODE));
    load_inode(f, inode_num);
    
    // permission
    char permission[11]; permission[10] = '\0';
    if(f->permission & 0x0020)
        permission[0] = 'd';
    else
        permission[0] = '-';

    for(int i = 1; i <= 9; i ++) permission[i] = '-';

    int p_pos = 1; 
    int b_pos = 8;
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'r';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'w';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'x';

    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'r';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'w';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'x';

    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'r';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'w';
    if((f->permission >> (b_pos --)) & 1) permission[p_pos ++] = 'x';

    printf("%s ",permission);

    printf("%s %s ", "martin", "martin");

    printf("%5u ", f->len);
    printf("%5u ", f->block_num);

    char * time_str = ctime(&f->st_mtime);
    time_str[strlen(time_str)-1] = '\0';
    printf("%s ", time_str);

    // free the inode
    free(f);
}





void prase_command(char * command){
        args_count = 0;
        // 假定空白字符的唯一的分隔符
        int cmd_s, cmd_e;
        if(command[strlen(command) - 1] == '\n') 
            command[strlen(command) - 1] = '\0';
        if(DEBUG_TERMINAL){
            printf("get command [%s]\n", command);
        }
        cmd_s = 0;
        while(command[cmd_s] != '\0'){
            while(command[cmd_s] != '\0'){
                if(command[cmd_s] != ' ') break;
                cmd_s ++; 
            }  
            cmd_e = cmd_s;
            while(command[cmd_e] != ' ' && command[cmd_e] != '\0') cmd_e ++;

            char t = command[cmd_e];
            command[cmd_e] = '\0';
            strcpy(args_contend[args_count ++], command + cmd_s);
            command[cmd_e] = t;

            cmd_s = cmd_e;
        }
}


    
