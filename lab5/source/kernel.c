#include "../header/kernel.h"
#include "../header/tools.h"
#include "../header/config.h"
#include "../header/debug.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#define FILE_NAME_100M "/home/martin/software/fs"
#define SIZE_1M (1024 * 1024)


struct fs fs_config;
struct free_node super_free_node; // 写入为data 区间的第一个区间
struct inode root_inode;
FILE* fptr;
struct buffer BUF;
struct inode curDir;
struct cur_path CUR_PATH;
struct inode * cur_dir;
char command[1000];
int args_count;
char args_contend[10][100];
enum file_type open_file_type;
int RE_INSTALL = 0;
int DEBUG_APP = 0;

void install_fs(){
    fptr = fopen(FILE_NAME_100M, "r+");
    if(fptr == NULL || DEBUG_FS || RE_INSTALL){
        
        // 创建的一个100 M 的文件
        if(!DEBUG_FS && fptr == NULL){
            fptr = fopen(FILE_NAME_100M, "a+");
            char stupid[SIZE_1M];
            for(int i = 0; i < 100; i++)
                fwrite(stupid, sizeof(char), sizeof(stupid), fptr);
            fclose(fptr);
        }

        // 写入超级节点
        fptr = fopen(FILE_NAME_100M, "rb+");
        fs_config.BLOCK_SIZE = 512;
        fs_config.BLOCK_NUM = 204800;
        fs_config.I_NODE_NUM = fs_config.BLOCK_NUM / 4;
        fs_config.I_NODE_BLOCKS = 
            fs_config.I_NODE_NUM * sizeof(struct inode) / fs_config.BLOCK_SIZE + 1;
        fs_config.DATA_BLOCKS_START = 1 + fs_config.I_NODE_BLOCKS;
        fseek(fptr , 0 , SEEK_SET );
        fwrite(&fs_config, sizeof(fs_config), 1, fptr);

        if(DEBUG_FS) print_fs();

        // data block 第一个位置设置为super_free_node 存储位置
        // 并且将所有磁盘块号添加到
        // 将数据super_free_node 存盘
        super_free_node.free_num = 1;
        super_free_node.s_free[0] = 0;
        for(Integer i = fs_config.DATA_BLOCKS_START + 1; i < fs_config.BLOCK_NUM; i++){
            free_block(i);
        }
        int head_addr =  fs_config.BLOCK_SIZE * fs_config.DATA_BLOCKS_START;
        fseek(fptr, head_addr, SEEK_SET);
        fwrite(&super_free_node, sizeof(super_free_node), 1, fptr);


        // inode table 中间的第一个位置写入 根节点数据inode
        inode_cons(&root_inode, IS_DIR);
        root_inode.inode_num = 0;
        root_inode.next_free = 1;
        fseek(fptr, fs_config.BLOCK_SIZE , SEEK_SET );
        fwrite(&root_inode, sizeof(root_inode), 1, fptr);
        
        if(DEBUG_FS) print_inode(&root_inode);

        // 之后位置写入链表关联的数据
        INODE c_inode;
        for (Integer i = 1; i < fs_config.I_NODE_NUM; i++){
            c_inode.next_free = i != fs_config.I_NODE_NUM - 1 ? i + 1 : 0;
            c_inode.inode_num = i;
            fwrite(&c_inode, sizeof(INODE), 1, fptr);
        }
    }
    fclose(fptr);
}


int fs_boot(){
    if(DEBUG_FS) printf("start the fs_boot\n");
    // open the file
    fptr = fopen(FILE_NAME_100M, "rb+");
    assert(fptr != NULL);

    // read fs_config
    fread(&fs_config, sizeof(fs_config), 1, fptr);
    if(DEBUG_FS) print_fs();

    // read super_node
    fseek(fptr, fs_config.BLOCK_SIZE * fs_config.DATA_BLOCKS_START, SEEK_SET);
    fread(&super_free_node, sizeof(super_free_node), 1, fptr);

    // read root inode
    fseek(fptr, fs_config.BLOCK_SIZE, SEEK_SET);
    fread(&root_inode, sizeof(root_inode), 1, fptr);
    if(DEBUG_FS) print_inode(&root_inode);

    // set cur dir
    cur_dir = &root_inode;

    // 维护cur_path
    cd_root();

    // problem check will be added soon
    return 0;
}



int fs_shutdown(){
    // super_free_node 存盘
    fseek(fptr, fs_config.BLOCK_SIZE * fs_config.DATA_BLOCKS_START, SEEK_SET);
    fwrite(&super_free_node, sizeof(super_free_node), 1, fptr);

    // fs_config 之后没有读写能力， 所以没有必要存储
    // 缓冲区 暂时没有存盘的必要的

    save_inode(cur_dir);
    save_inode(&root_inode);
    if(DEBUG_APP) print_inode(cur_dir);
    fclose(fptr);
    return 0;
}



int alloc_block(){
    if(DEBUG_APP) print_super_free_node(0, -1);
    super_free_node.free_num --;
    if(super_free_node.free_num > 0){
        // 含有直接相连的空闲块
        return super_free_node.s_free[super_free_node.free_num];
    }else if(super_free_node.s_free[0] > 0){
        // 将指向块添加到
        int block_id = super_free_node.s_free[0];
        fseek(fptr, fs_config.BLOCK_SIZE * block_id, SEEK_SET);
        fread(&super_free_node, sizeof(struct free_node), 1, fptr);

        return block_id; 
    }
    return 0;
}

int free_block(int block_id){
    if(DEBUG_APP) print_super_free_node(1, block_id);
    
    if(super_free_node.free_num == 100){
        // super_block 中间数据满， 将空闲区间放置 super block 的数据
        fseek(fptr, fs_config.BLOCK_SIZE * block_id, SEEK_SET);
        fwrite(&super_free_node, sizeof(super_free_node), 1, fptr);
        // 处理super_block 重新回复为free_num = 1
        super_free_node.free_num = 1;
        super_free_node.s_free[0] = block_id;
    }else{
        super_free_node.s_free[super_free_node.free_num] = block_id;
        super_free_node.free_num ++;
    }
    return 0;
}
