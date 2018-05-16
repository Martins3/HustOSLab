#ifndef DEBUG_H
#define DEBUG_H
#include "./config.h"

#define ENABLE_DEBUG 0
#define DEBUG_FS 0
#define DEBUG_IO 0
#define DEBUG_TERMINAL 0
#define COMPLETE_INODE 0


#define FUCK printf("HERE\n");

void print_fs();
void print_super_free_node(int disk_id, int mode);

void print_file_blcoks(fFile node);

void print_cur_path();


void print_inode(fFile node);
void print_DIR_FILE(DIR_FILE * f);
void print_read_file_config(Integer size, Integer count, fFile f);


void print_load_blocks();
#endif // !1DEBUG_H
