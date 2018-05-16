#ifndef CONFIG_H
#define CONFIG_H

#include<time.h>
#include<stdio.h>


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define VIM_BUF_PATH "/home/martin/X-Brain/Notes/Atom/notes/os/project/fs/buf.txt"
/**
 * fs 0
 * inode_table x
 * free_node_head x + 1
 */
struct fs{
    unsigned int BLOCK_SIZE;
    unsigned int BLOCK_NUM;
    unsigned int I_NODE_NUM; // how many indoe
    unsigned int I_NODE_BLOCKS; // the nums of blcoks alloc for i_node table
    unsigned int  DATA_BLOCKS_START; // the root dir inode 0, should be set as 0. but disk blocks may not
};

/**
 * 1. for every empty block, there is just one struct for it !
 * 2. the super free_node is in the ram
 * 3. there is a special area for super free_node
 *  
 * 对于 0 的位置添加数据是 管理区间
 */
struct free_node{
    int free_num;
    int s_free[100];
};


/**
 * assume every 1k, there is a inode in the table
 * 计算inode 含有偏移量， 但是磁盘块号没有
 * 
 * inode 既可以文件磁盘管理， 也是读取管理的
 * 
 * inode 使用链表的方法实现空闲的inode 的管理
 * 
 * 使用前面 permission的前面的9为 p
 * inode 中间的含有父节点的inode， 但是为cd .. 的
 * 方便的 需要添加进去
 */
struct inode{
    int inode_num;
    int permission;
    int u_id;
    int g_id;
    time_t st_mtime; // modify

    unsigned int len; // bytes
    unsigned int block_num; // blocks
    unsigned int ptr_pos;

    unsigned int i_addr[14]; 

    /**
     * 管理空闲inode 数据
     * 当该inode 被占用的时候，为0
     * 当root_inode 为0， 表示没有空闲inode 可以使用
     * 回收的时候的添加的数据的
     */
    unsigned int next_free;
    unsigned int parent;
};
/**
 * the directory file contains a list of dir_file
 * only the root doesn't have the parent file !
 */
struct dir_file{
    int inode_num;
    char file_name[48]; // ascii supported
    int type;
    /**
     *  -1 means parent dir 
     *  0 means file  
     *  1 means dir
     */
};

/**
 * 1. 首先确保数据是自己的
 * 2. 
 */
#define BUF_NUM 8
#define BUF_SIZE 512 * BUF_NUM
struct buffer{
    char data[BUF_SIZE];    
    unsigned int ptr_pos;
    
    // indicate 多少个写入
    int write_num;
    // disk block num
    unsigned int blocks[BUF_NUM];

};

/**
 * 文件定位创建枚举
 */
enum file_pos{
    START, END
};

// 文件类型
enum file_type{
    IS_DIR, IS_FILE
};

/**
 * 假设文件名不会超过1000
 * 从根节点到达当前的位置不会有100 级别
 * 
 * fs_boot() 初始化
 */
struct cur_path{
    char path[1000];
    unsigned int inodes[100];
    unsigned int p_inodes;

};

/**
 * declare important data store in ram
 * the data is declare in the kernel.c
 * buf 是实现将整合的数据整合的关键
 * buf 
 */
extern struct fs fs_config;
extern struct free_node super_free_node;
extern FILE * fptr;
extern struct buffer BUF;

extern struct inode root_inode;
extern struct inode curDir;
extern struct cur_path CUR_PATH; 
extern struct inode * cur_dir;
extern char command[1000];
extern int args_count;
extern char args_contend[10][100];
extern enum file_type open_file_type;
extern int RE_INSTALL;
extern int DEBUG_APP;
/**
 * define some type !
 */
typedef struct inode INODE;
typedef struct inode * fFile;
typedef unsigned int Integer;
typedef struct dir_file DIR_FILE;
typedef enum file_pos F_POS;

// 当前目录pointer, 配合get_cur_dir 使用
#endif