/**
 * 1. int 的大小 随着os 变化是否处理 ！
 * 2. 
 */
#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include "config.h"
/**
 * 创建100M 的文件出来用于模拟文件系统
 *  申请 100M 的文件 
 *   
 *  确定总的空闲块数目 等等super block 的参数， 然后
 *  将空闲块的进行管理, 并且确定 super free_node 的位置
 * 
 *  
 *  
 *  使用一个块来实现的根节点的数据保存
 *  
 *  将对应的内容写入到对应文件中间, 如果成功返回值为 文件指针
 */
void install_fs();

/**
 * 打开文件系统的初始化， 将重要数据添加到Ram 中间
 * 1. super 空闲块 
 * 2. 根节点i_node
 * 
 * 3. print some info to obey
 */
int fs_boot();
int fs_shutdown();

/**
 * free block
 * blcok id will be checked to make sure in range
 * block not started at 0, use the absolute offset
 */
int free_block(int block_id);

/**
 * return the block in used !
 * when the "disk" is full, return 0, for the 0 is reserve for
 * super block
 */
int alloc_block();

#endif
