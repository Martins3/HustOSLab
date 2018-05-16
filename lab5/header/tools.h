#ifndef TOOLS_H
#define TOOLS_H
#include "config.h"

void file_seek(fFile file, F_POS p);
void seek(unsigned int block);

void inode_cons(fFile f, enum file_type t);
// 根据文件的位置获取文件inode
void load_inode(fFile fp, unsigned int inode_num);
// 将inode写入到制定的位置
void save_inode(fFile fp);

// 提供 app cd 的内核版本
void cd_inode(unsigned int inode_num, const char * file_name);
void cd_out();
void cd_root();

// 向用户的提供文件的详细信息
void print_file_msg(Integer inode_num);

int free_file(fFile);

// 将命令 转化为的条
void prase_command(char * command);

#endif //  TOOLS.H