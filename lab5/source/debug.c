#include "../header/debug.h"
#include "../header/config.h"
#include "../header/tools.h"

// debug fs
void print_fs(){
    printf("block size : %u\n \
blocks num : %u\n \
inode num : %u\n \
inode blocks : %u \n \
data block start ： %u \n" ,\
fs_config.BLOCK_SIZE, fs_config.BLOCK_NUM, \
fs_config.I_NODE_NUM, fs_config.I_NODE_BLOCKS, \
fs_config.DATA_BLOCKS_START);
}

void print_super_free_node(int mode, int disk_id){
    if(mode == 1){
        printf("free a node :  %d\n: ", disk_id);
    } else{
        printf("alloc a node : %d\n", disk_id);
    }
    printf("s_free 为 %d\n", super_free_node.free_num);

    
    // printf("对应的磁盘的块号的是：\n");
    // for(int i = 0; i < 100; i ++){
    //     printf("%d ", super_free_node.s_free[i]);
    //     if((i + 1) % 10 == 0) printf("\n");
    // }
}



void print_file_blcoks(fFile node){

    for(Integer i = 0; i < node->block_num; i++){
        Integer disk_id; 

        if(i <= 10){
            disk_id = node->i_addr[i];
        }else{
            int b_n = fs_config.BLOCK_SIZE / sizeof(int);
            int x = (i - 11) / (b_n) + 11;
            int y = (i - 11) % (b_n);
            
            seek(node->i_addr[x]);
            fseek(fptr, y * sizeof(int), SEEK_CUR);
            fread(&disk_id, sizeof(int), 1, fptr);
        }
        printf("%u -> %u\n", i, disk_id);
    }
}

void print_cur_path(){
    printf("PATH INFO\n");
    printf("path name %s\n", CUR_PATH.path);
    printf("dir level %d\n", CUR_PATH.p_inodes);
}


// debug IO

void print_DIR_FILE(DIR_FILE * f){
    printf("inode num %d\n", f->inode_num);
    
    if(f->type == -1){
        printf("Dir name %s\n", "..");
    }else if(f->type == 0){
        printf("file name %s\n", f->file_name);
    }else if(f->type == 1){
        printf("Dir name %s\n", f->file_name);
    }
    printf("\n");
}

void print_inode(fFile node){
    if(node == NULL){
        printf("\nFailed to open the inode info\n");
        return;
    }
    printf("\n----------------\n");
    printf("INODE INFO\n");
    printf("inode_num :  %d\n", node->inode_num);

    printf("next_free :  %d\n", node->next_free);
    printf("len : %u\n", node->len);
    printf("blocks : %u\n", node->block_num);

    printf("file blocks are :\n");
    print_file_blcoks(node); 
    printf("parent inode num %d\n",node->parent);
    printf("----------------\n\n");
}

void print_read_file_config(Integer size, Integer count, fFile f){
    printf("\ntry to read/write file: \n");
    printf("inode_num :  %d\n", f->inode_num);
    printf("begin %u \n", f->ptr_pos);
    printf("size %u \n", size);
    printf("count %u \n\n", count);
}

void print_load_blocks(){
    printf("load blocks are : \n");
    for (int i = 0; i < BUF_NUM; i++) {
        printf("%d %d \n", i, BUF.blocks[i]);
        if(!BUF.blocks[i]) break;
    }
    printf("\n");
}