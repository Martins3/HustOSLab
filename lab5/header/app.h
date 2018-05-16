#ifndef APP_H
#define APP_H
#include "config.h"



int touch(const char * file_name);
int mkdir(const char * file_name);
int rm(const char * file_name, enum file_type t);

// 应该是的唯一可以形成的数据不一致的来源
int cd(const char * dir_name);
int mv(int inode_num, int des_dir_inode_num);

int ls(int l);
void tree(); 
int tree_rec(fFile dir, int depth, int * file_num, int * dir_nums);

void pwd(int is_abs);

void terminal();

// 首先检查的文件名的存在
void cat(const char * file_name);

// 首先删除文件 然后添加的文件
void echo(const char * file_name, char * contend);

// 调用 vim
void vim(const char * file_name);

#endif // !1APP_H