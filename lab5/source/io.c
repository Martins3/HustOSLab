#include "../header/config.h"
#include "../header/io.h"
#include "../header/tools.h"
#include "../header/kernel.h"
#include "../header/debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int list_dir(fFile dir){
    DIR_FILE f;
    if(dir == NULL) return -1;
    DIR_FILE* files = (DIR_FILE*) malloc(dir->len);
    
    dir->ptr_pos = 0;
    read_file(files, sizeof(DIR_FILE), dir->len / sizeof(DIR_FILE), dir);


    fFile target_file = NULL;
    for(Integer i = 0; i < dir->len / sizeof(DIR_FILE); i ++){
        memcpy(&f, &files[i], sizeof(f));
        print_DIR_FILE(&f);
    }

    free(files);
    return 0;
}


void clear(fFile file){
    // 释放所有磁盘的位置
    for(Integer i = 0; i < file->block_num; i++){
        Integer disk_id; 

        if(i <= 10){
            disk_id = file->i_addr[i];
        }else{
            int b_n = fs_config.BLOCK_SIZE / sizeof(int);
            int x = (i - 11) / (b_n) + 11;
            int y = (i - 11) % (b_n);
            
            seek(file->i_addr[x]);
            fseek(fptr, y * sizeof(int), SEEK_CUR);
            fread(&disk_id, sizeof(int), 1, fptr);
        }
        free_block(disk_id);
    }

    file->len = 0;
    file->block_num = 0;
    file->ptr_pos = 0;
}

void delete_dir_item(const char * dir_name){
    // 读出数据 然后将前后的写入到其中
    // 遍历当前目录的所有目录项
    
    DIR_FILE f;
    DIR_FILE* files = (DIR_FILE*) malloc(cur_dir->len);
    
    cur_dir->ptr_pos = 0;
    read_file(files, sizeof(DIR_FILE), cur_dir->len / sizeof(DIR_FILE), cur_dir);
    Integer item_num = cur_dir->len / sizeof(DIR_FILE);
    clear(cur_dir); // 将当前文件清空

    int find_assert = 0;
    for(Integer i = 0; i < item_num; i ++){
        memcpy(&f, &files[i], sizeof(f));
        if(strcmp(f.file_name, dir_name) == 0){
            // 将前后的数据写入的到中间
            file_seek(cur_dir, END);
            write_file(files, sizeof(DIR_FILE), i, cur_dir);
            file_seek(cur_dir, END);
            write_file(files + i + 1, sizeof(DIR_FILE), item_num - i - 1, cur_dir);
            find_assert = 1;
            break; 
        }
    }
    assert(find_assert);
    free(files);
}

int delete_inode(Integer inode_num){
    // 释放inode
    fFile file = (fFile)malloc(sizeof(INODE));
    load_inode(file, inode_num);
    file->next_free =  root_inode.next_free;
    root_inode.next_free = inode_num;
    if(DEBUG_APP) {
        printf("delete inode %u\n", inode_num);
        print_inode(&root_inode);
    }

    save_inode(file); // 该节点已经释放， len 信息不需要修改

    clear(file);
    free(file);
    return 1;
}


    
int delete_recur(Integer inode_num, enum file_type t){
    fFile dir = (fFile) malloc(sizeof(INODE));
    load_inode(dir, inode_num);
    if(DEBUG_APP) {
        printf("将要处决的inode !\n");
        print_inode(dir);
        printf("包含的内容\n");
        list_dir(dir);
    }
    DIR_FILE f;
    DIR_FILE* files = (DIR_FILE*) malloc(dir->len);

    dir->ptr_pos = 0;
    read_file(files, sizeof(DIR_FILE), dir->len / sizeof(FILE), dir);

    for(Integer i = 0; i < dir->len / sizeof(DIR_FILE); i ++){
        memcpy(&f, &files[i], sizeof(f));
        if(DEBUG_APP && f.type != -1){
            printf("当前inode %u\n", inode_num);
            printf("删除的 inode [%d]  => delete post order \n", f.inode_num);
            print_DIR_FILE(&f);
        }
        if(f.type == 1){
            // 进一步删除的文件夹
            delete_recur(f.inode_num, IS_DIR); 
        }else if(f.type == 0){
            delete_inode(f.inode_num);
        }

    }


    delete_inode(inode_num);
    free(files);
    free(dir);
    return 100;
}

int delete_unix_file(const char * unix_file, enum file_type t){
    // 确定当前文件是否存在的
    fFile file  = open_file(unix_file);
    if(t != open_file_type) return -1;
    if(file != NULL){
        // 如果是文件夹， 那么首先需要 将 持有的数据释放 掉
        assert(file->inode_num);
        if(t == IS_DIR) {
            delete_recur(file->inode_num, t);
        }
        if(t == IS_FILE) delete_inode(file->inode_num);
        delete_dir_item(unix_file);
        close_file(file);
        return 1;
    }
    printf("%s%s does't exit !", KRED, unix_file);
    return 0; 
}


int new_unix_file(const char * unix_file, enum file_type t){
    fFile file  = open_file(unix_file);
    if(file == NULL){
        file = (fFile)malloc(sizeof(INODE));

        // 获取空闲的inode 
        Integer inode_num = root_inode.next_free;
        load_inode(file, inode_num);
        root_inode.next_free = file->next_free;
        inode_cons(file, t);


        // 将添加该目 或者 文件 添加到curDir中间
        DIR_FILE * dir = (DIR_FILE *)malloc(sizeof(DIR_FILE));
        dir->inode_num = inode_num;
        dir->type = t == IS_DIR ? 1 : 0; // 文件或者目录
        strcpy(dir->file_name, unix_file);


        file_seek(cur_dir, END);
        write_file(dir, sizeof(DIR_FILE), 1, cur_dir);

        // 记录自己的parent 
        file->parent = cur_dir->inode_num;

        // 如果是目录， 那么添加目录项到达中间的去
        if(t == IS_DIR){
            dir->inode_num = cur_dir ->inode_num;
            dir->type = -1;
            strcpy(dir->file_name, ".."); 
            file_seek(file, END);
            write_file(dir, sizeof(DIR_FILE), 1, file);
        }

        // 写回 和 释放 
        save_inode(file);
        
        free(file);
        free(dir);
        return 1;
    }
    return 0;
}

fFile open_file(const char * file_name){
    // 获取当前的目录所有数据到达内存中
    if(DEBUG_IO) printf("try to open %s\n", file_name);
    DIR_FILE f;
    DIR_FILE* files = (DIR_FILE*) malloc(cur_dir->len);
    
    cur_dir->ptr_pos = 0;
    read_file(files, sizeof(DIR_FILE), cur_dir->len / sizeof(DIR_FILE), cur_dir);


    fFile target_file = NULL;
    for(Integer i = 0; i < cur_dir->len / sizeof(DIR_FILE); i ++){
        memcpy(&f, &files[i], sizeof(f));
        if(!strcmp(f.file_name, file_name)){
            target_file = (fFile)malloc(sizeof(INODE));
            open_file_type = f.type == 0 ? IS_FILE : IS_DIR;
            load_inode(target_file, f.inode_num);
            target_file->ptr_pos = 0;
            break; 
        }
    }

    free(files);
    if(DEBUG_IO) print_inode(target_file);
    return target_file; 
}


int close_file(fFile file){
    if(file != NULL) free(file);
    return 0;
}


int read_file(void * ptr, Integer size,
Integer count, fFile file){
    if(file == NULL){
        assert(0);
        return -1;
    }

    if(DEBUG_IO) print_read_file_config(size, count, file);

    assert(file->len >= file->ptr_pos);
    
    Integer remain = file->len - file->ptr_pos;
    Integer already_read = 0;

    char * obp = (char *)ptr;

    // 到达文件结束 或者 预期数目读取完的时候终止
    while(remain != 0  && already_read != count * size){
        load_block(file);
        Integer s = file->ptr_pos % fs_config.BLOCK_SIZE;
        
        // 选择三者最小值
        Integer e = sizeof(BUF.data);
        if(e > remain + s) e = remain + s;
        if(e > (count * size - already_read) + s)
            e = (count * size - already_read) + s;

        // 从缓冲区间读取数据到 ptr 中间
        memcpy(obp, &BUF.data[s], e - s);

        // update 
        Integer cpy_len = e - s;
        obp += cpy_len;
        file->ptr_pos += cpy_len;
        remain -= cpy_len;
        already_read += cpy_len;
    }
    return already_read / size;
}

int write_file(void * ptr, Integer size,
Integer count, fFile file){
    if(count == 0) return 0;
    if(DEBUG_IO) print_read_file_config(size, count, file);
    if(file == NULL){
        assert(0);
        return -1;
    }

    // 首先检查磁盘是否可以写下当前文件的

    assert(file->len >= file->ptr_pos);
    Integer already_write = 0;
    Integer new_len = file->ptr_pos + count * size;
    file->len = new_len > file->len ? new_len : file->len;


    char * obp = (char *)ptr;
    
    
    while(already_write != count * size){
        Integer s = file->ptr_pos % fs_config.BLOCK_SIZE;
        
        Integer e = sizeof(BUF.data);
        if(e > (count * size - already_write) + s) 
            e = (count * size - already_write) + s;
        // 从缓冲区间读取数据到 ptr 中间
        load_block(file); // 防止出现的数据覆盖的
        memcpy(&BUF.data[s], obp, e - s);

        BUF.write_num = e / fs_config.BLOCK_SIZE;
        if(e % fs_config.BLOCK_SIZE) BUF.write_num ++;
        write_block(file);
        
        // update 
        Integer cpy_len = e - s;
        obp += cpy_len;
        file->ptr_pos += cpy_len;
        already_write += cpy_len;
    }
    
    // 数据存盘
    save_inode(file);
    return already_write / size;
}


int write_block(fFile file){
    get_blocks_nums(file);
    int i;
    for (i = 0; i < BUF.write_num; i++) {
        int disk_num;
        if(BUF.blocks[i] == 0){
            disk_num = extend_file(file);
        } else{
            disk_num = BUF.blocks[i];  
        }
        seek(disk_num);
        fwrite(&BUF.data[i * fs_config.BLOCK_SIZE], 
        sizeof(char) * fs_config.BLOCK_SIZE, 1, fptr); 
    }
    return i; 
}

// 读写位置含有 问题 吗 ？
int load_block(fFile file){
    get_blocks_nums(file);
    int i;
    // memset(BUF.blocks, 0, sizeof(BUF.blocks)); // useless
    for (i = 0; i < BUF_NUM; i++) {
        if(!BUF.blocks[i]) break;
        seek(BUF.blocks[i]);
        fread(&BUF.data[i * fs_config.BLOCK_SIZE], 
        sizeof(char) * fs_config.BLOCK_SIZE, 1, fptr); 
    }
    return i; 
}

void get_blocks_nums(fFile file){
    Integer start_i = file->ptr_pos / fs_config.BLOCK_SIZE;
    memset(BUF.blocks, 0, sizeof(BUF.blocks));

    // 三个间接 链接 
    for(Integer i = start_i; i < BUF_NUM + start_i; i++){
        if(i >= file->block_num) break;

        if(i <= 10){
            BUF.blocks[i - start_i] = file->i_addr[i];
        }else{
            int b_n = fs_config.BLOCK_SIZE / sizeof(int);
            int x = (i - 11) / (b_n) + 11;
            int y = (i - 11) % (b_n);
            
            seek(file->i_addr[x]);
            fseek(fptr, y * sizeof(int), SEEK_CUR);
            fread(&BUF.blocks[i - start_i], sizeof(int), 1, fptr);
        }
    }

}

int extend_file(fFile file){
    Integer add_num = file -> block_num;

    if(add_num <= 10) {
        file->i_addr[add_num] = alloc_block();
        if(DEBUG_APP)printf("extend file %d %d\n",add_num, file->i_addr[add_num]);
        file->block_num ++;
        return file->i_addr[add_num];
    }

    // 下标为 11 12 13 ， 使用间接 
    else{
        int b_n = fs_config.BLOCK_SIZE / sizeof(int);
        int x = (add_num - 11) / (b_n) + 11;
        int y = (add_num - 11) % (b_n);
        if(y == 0) file->i_addr[x] = alloc_block();


        seek(file->i_addr[x]);
        fseek(fptr, y * sizeof(int), SEEK_CUR);
        int b_id = alloc_block();
        fwrite(&b_id, sizeof(int), 1, fptr);

        // update the file 
        file->block_num ++;
        return b_id;
    }
    return 0;
}