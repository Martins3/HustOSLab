#ifndef IO_H
#define IO_H
#include "../header/config.h"



// 打印目录内容
int list_dir(fFile dir);
/**
 * 将文件长度设置为0
 * blocks 数目设置为0
 * 释放所有的磁盘块
 */
void clear(fFile file);
// 用于删除 当前 目录文件的中间的一个目录项
void delete_dir_item(const char * dir_name);
/**
 * 用于释放inode_num 和 文件的控制的所有磁盘块
 * 
 * 释放inode_num修改 磁盘数据链表
 * 使用循环释放成组链接法的数据的
 */
int delete_inode(Integer inode_num);
/**
 * 递归的删除i_num 下面的所有文件， 返回成功删除的数目
 * 由于inode 中间的不含有的文件类型， 所以需要添加判断 
 * 
 * 最上面的处理涉及到的和cur_dir 打交道， 之后则是递归删除
 * 参数表示 文件夹的inode_num, 释放参数的不是归本函数管理！
 * 
 */
int delete_recur(Integer inode_num, enum file_type t);
// 删除文件 或者 目录
int delete_unix_file(const char * unix_file, enum file_type t);
// 创建文件或者目录,
int new_unix_file(const char * unix_file, enum file_type t);
// 根据文件名返回的 文件对应的inode， open file 需要和close file 对应
fFile open_file(const char * file_name);
// 释放inode 的空间， 同时将inode 写入到对应位置
int close_file(fFile file);

int read_file(void * ptr, unsigned int size,
unsigned int conut, fFile file);

int write_file(void * ptr, unsigned int size,
unsigned int conut, fFile file);

/**
 * 支持read_file 和 write_file 的函数，
 * 当调用函数之后， 缓冲区间 最低块正好包含的 ptr
 * 让用户的每次读取都是从缓冲区中间读取的, 
 * 当read_file 或者 write_file 自己读写过程中间ptr_pos调整位置
 *  1. 每次使用 8 个 磁盘块实现缓冲
 *  2. 需要处理 多级间接
 *  3. 前面的11 个直接， 后面的为 1 2 3级别
 * 
 * 方法:
 *  1. 释放block 的时候， 多级 的释放， 分配
 *  2. 处理 int 的长度问题
 */
int write_block(fFile file); // 返回成功写入的数目， 不为0
int load_block(fFile file); // 返回成功加载数目， 数目不可以为0

/**
 * 向 write_block 和 load_block 提供磁盘
 * 当没有8 个的时候，该磁盘号码为 0 
 * 提供磁盘块号是绝对的， 使用 seek() 可以食用 
 */
void get_blocks_nums(fFile file);

/**
 * 为文件添加一个块，当文件太大或者 没有空间的时候， 返回 0
 * 如果含有, 并且返回 分配的磁盘块号
 * 
 * 修改i_addr 数值
 * 修改文件的两个容量
 * 
 * 注意： 操作上， 数据的同步 
 * 
 */
int extend_file(fFile file);


#endif // !1IO_H
