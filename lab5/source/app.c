#include "../header/io.h"
#include "../header/kernel.h"
#include "../header/config.h"
#include "../header/tools.h"
#include "../header/debug.h"
#include "../header/app.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int touch(const char * file_name){
    if(DEBUG_TERMINAL) printf("touch file [%s]", file_name);
    return new_unix_file(file_name, IS_FILE);
}

int mkdir(const char * file_name){
    return new_unix_file(file_name, IS_DIR);
}

int rm(const char * file_name, enum file_type t){
    return delete_unix_file(file_name, t);
}


void vim(const char * file_name){
    fFile f = open_file(file_name);
    if(f == NULL){
        touch(file_name);
        f = open_file(file_name);
    }

    if(open_file_type == IS_DIR){
        printf("%s%s", KRED, "vim can not apply to directory !\n");
        return;
    }

    if(DEBUG_APP) printf("vim 读入的数据长度为%u\n", f->len);

    // copy the contend to the buf
    char * contend = (char *)malloc(f->len);
    read_file(contend, sizeof(char), f->len, f);
    contend[f->len] = EOF;
    FILE * buf = fopen(VIM_BUF_PATH, "wb");
    if(f->len){
        fwrite(contend, sizeof(char), f->len - 1, buf);
    }
    fclose(buf);

    free(contend);

    // open vim
    printf("start vim !\n");
    char * args[] = {"vim", VIM_BUF_PATH, NULL};
    int status;
    if(!fork()){
        execvp("vim", args);
    }
    // wait(&status);
    printf("back to the main process");


    buf = fopen(VIM_BUF_PATH, "rb");
    fseek(buf, 0, SEEK_END);
    long fsize = ftell(buf);
    fseek(buf, 0, SEEK_SET);
    contend = (char *)malloc(fsize + 1);
    fread(contend, fsize, 1, buf);
    fclose(buf);
    contend[fsize] = '\0';

    clear(f);

    if(DEBUG_APP) printf("从buf.txt 中间 向 该文件 写入数据长度为%lu\n", fsize + 1);
    write_file(contend, sizeof(char), fsize + 1, f);
    save_inode(f);

    close_file(f);
}

void tree(){
    // 不要采用变化的文件
    fFile f = (fFile) malloc(sizeof(INODE));
    memcpy(f, cur_dir, sizeof(INODE));
    int file_num = 0;
    int dir_num = 0;
    tree_rec(f, 0, &file_num, &dir_num);
    free(f);
    printf("there are %d dirs and %d files \n", dir_num, file_num);
}

int tree_rec(fFile dir, int depth, int * file_num, int * dir_num){
    DIR_FILE f;
    DIR_FILE* files = (DIR_FILE*) malloc(dir->len);

    dir->ptr_pos = 0;

    read_file(files, sizeof(DIR_FILE), dir->len / sizeof(DIR_FILE), dir);
    int indent = depth * 10;
    for(Integer i = 0; i < dir->len / sizeof(DIR_FILE); i ++){
        memcpy(&f, &files[i], sizeof(f));

        if(f.type == 0){
            printf("%*s%s\n",indent,  KGRN, f.file_name);
            (*file_num) ++;
        }
        if(f.type == 1){
            printf("%*s%s\n",indent, KMAG, f.file_name);
            (*dir_num) ++;
            // 递归
            fFile sec_dir = (fFile)malloc(sizeof(INODE));
            load_inode(sec_dir, f.inode_num);
            tree_rec(sec_dir, depth + 1, file_num, dir_num);
            free(sec_dir);
        }
    }

    free(files);
    // 返回的整个目录的文件的数目
    return 0;
}


int ls(int l){
    if(DEBUG_IO) print_inode(cur_dir);
    DIR_FILE f;
    DIR_FILE* files = (DIR_FILE*) malloc(cur_dir->len);

    cur_dir->ptr_pos = 0;
    read_file(files, sizeof(DIR_FILE), cur_dir->len / sizeof(DIR_FILE), cur_dir);

    for(Integer i = 0; i < cur_dir->len / sizeof(DIR_FILE); i ++){
        memcpy(&f, &files[i], sizeof(f));
        // complex msg
        if(l == 1){
            if(f.type == 0){
                print_file_msg(f.inode_num);
                printf("%s%s\n", KGRN, f.file_name);
            } else{
                print_file_msg(f.inode_num);
                printf("%s%s\n", KMAG, f.file_name);
            }
        }
        // simple msg
        else if(l == 0){
            if(f.type == 0)
                printf("%s%s\n", KGRN, f.file_name);
            if(f.type == 1)
                printf("%s%s\n", KMAG, f.file_name);
        }
        // print the inode msg
        else if(l == 2){
            if(DEBUG_IO || COMPLETE_INODE){
                fFile the_f = (fFile)malloc(sizeof(INODE));
                load_inode(the_f, f.inode_num);
                print_inode(the_f);
                free(the_f);
            }
            if(f.type == 0)
                printf("%s%s %d\n", KGRN, f.file_name, f.inode_num);
            if(f.type == 1 || f.type == -1)
                printf("%s%s %d\n", KMAG, f.file_name, f.inode_num);
        }
    }

    free(files);
    return 0;
}

void pwd(int is_abs){
    if(is_abs){
        // 打印绝对路径
        printf("%s->%s %s : ", KGRN, KRED, CUR_PATH.path);
    }else{
        // 打印的父目录的路径
        // 使用定位符的实现的名称的确定的
        printf("%s->%s %s : ", KGRN, KRED, CUR_PATH.path);
    }
}


int cd(const char * dir_name){
    // 只要发生切换的目录， 那么立刻保存当前目录， 即时不成功
    save_inode(cur_dir);

    // 判断的是否为根目录
    if(strcmp(dir_name, "/") == 0){
        cur_dir = &root_inode;
        cd_root();
        return 1;
    }


    // 判断是否为父目录
    // 根的数据只有最后的才会写入到磁盘中间的
    if(strcmp(dir_name, "..") == 0){
        if(cur_dir->inode_num == 0) return 0;
        if(cur_dir->parent != 0){
            load_inode(&curDir, cur_dir->parent);
            cur_dir = &curDir;
        }else{
            cur_dir = &root_inode;
        }
        cd_out();
        return 1;
    }


    fFile d = open_file(dir_name);
    if(d == NULL) return 0;
    if(open_file_type == IS_FILE) return 0;
    cd_inode(d->inode_num, dir_name);
    memcpy(&curDir, d, sizeof(INODE));
    cur_dir = &curDir;
    close_file(d);
    return 1;
}

void cat(const char * file_name){
    fFile f = open_file(file_name);
    if(f == NULL){
        printf("%s%s", KRED, "No such file !\n");
        return;
    }

    if(open_file_type == IS_DIR){
        printf("%s%s", KRED, "cat can not apply to directory !\n");
        return;
    }
    char * contend = (char *) malloc(f->len);
    file_seek(f, START);
    read_file(contend, sizeof(char), f->len, f);
    if(DEBUG_APP){
        printf("读入的文件的长度为%d", f->len);
    }
    printf("%s%s", KCYN, contend);
    close_file(f);
}

void echo(const char * file_name, char * contend){
    fFile f = open_file(file_name);
    if(f == NULL){
        touch(file_name);
        f = open_file(file_name);
    }

    if(open_file_type == IS_DIR){
        printf("%s%s", KRED, "echo can not apply to directory !\n");
        return;
    }
    clear(f);
    write_file(contend, sizeof(char), strlen(contend) + 1, f);
    close_file(f);
}


void terminal(){
    // clc
    // printf("\e[1;1H\e[2J");
    pwd(1);
    while(fgets(command, 1000, stdin)){
        int error = 1;

        prase_command(command);
        if(args_count == 0) continue;


        if(strcmp(args_contend[0], "q") == 0){
            printf("Bye Bye !\n");
            break;
        }

        if(strcmp(args_contend[0], "clc") == 0){
            error = 0;
            if(args_count == 1){
                printf("\e[1;1H\e[2J");
            }else{
                error = 1;
            }
        }

        if(strcmp(args_contend[0], "rm") == 0){
            error = 0;
            if(args_count == 1){
                printf("rm: remove dir or file\n");
            }else if(args_count == 2){
                if(rm(args_contend[1], IS_FILE) == -1){
                    printf("Directory should be removed recursive\n");
                }
            }else if(args_count == 3 && strcmp(args_contend[1], "-r") == 0){
                if(rm(args_contend[2], IS_DIR) == -1){
                    printf("Just use rm to remove file\n");
                }
            }else{
                error = 1;
            }
        }

        if(strcmp(args_contend[0], "tree") == 0){
            error = 0;
            if(args_count == 1){
                tree();
            }else{
                error = 1;
            }
        }


        if(strcmp(args_contend[0], "ls") == 0){
            error = 0;
            if(args_count == 1){
                ls(0);
            }else if(args_count == 2 && strcmp("-l", args_contend[1]) == 0){
                ls(1);
            }else if(args_count == 2 && strcmp("-i", args_contend[1]) == 0){
                ls(2);
            }else{
                error = 1;
            }
        }

        if(strcmp(args_contend[0], "touch") == 0){
            error = 0;
            if(args_count == 1) printf("touch: create a file\n");
            if(args_count >= 2){
                for(int i = 1; i < args_count; i ++){
                    touch(args_contend[i]);
                }
            }
        }

        if(strcmp(args_contend[0], "echo") == 0){
            error = 0;
            if(args_count == 1) printf("echo: write to the file\n");
            if(args_count == 2) printf("%s%s\n",KMAG, args_contend[1]);
            else if(args_count == 4) echo(args_contend[3], args_contend[1]);
            else error = 1;
        }


        if(strcmp(args_contend[0], "mkdir") == 0){
            error = 0;
            if(args_count == 1) printf("mkdir : create a directory\n");
            if(args_count >= 2){
                for(int i = 1; i < args_count; i ++){
                    mkdir(args_contend[i]);
                }
            }
        }

        if(strcmp(args_contend[0], "cat") == 0){
            error = 0;
            if(args_count == 1) printf("cat : print the contend of the file\n");
            else if(args_count == 2) cat(args_contend[1]);
            else error = 1;

        }

        if(strcmp(args_contend[0], "init") == 0){
            error = 0;
            if(args_count > 1){
                error = 1;
                printf("%s%s", KRED, "No parameter needed !\n");
            }else{
                int t = DEBUG_APP;
                DEBUG_APP = 0;
                RE_INSTALL = 1;
                fs_shutdown();
                install_fs();
                fs_boot();
                DEBUG_APP = t;
            }
        }


        if(strcmp(args_contend[0], "chmod") == 0){
            error = 0;
            if(args_count == 1) printf("chmod : chmod \n");
            if(args_count == 2) {
                if(!cd(args_contend[1])){
                    printf("%s%s\n", KRED, "no such dir\n");
                }else{
                    printf("%s%s\n", KRED, "why !\n");
                }
            }
            if(args_count > 2) error = 1;
        }


        if(strcmp(args_contend[0], "cd") == 0){
            error = 0;
            if(args_count == 1) printf("cd : move to a dir\n");
            if(!cd(args_contend[1])){
                printf("%s%s\n", KRED, "no such dir\n");
            }
            if(args_count > 2) error = 1;
        }

        if(strcmp(args_contend[0], "vim") == 0){
            error = 0;
            if(args_count == 1) printf("vim : open the file with vim\n");
            if(args_count == 2) vim(args_contend[1]);
            if(args_count > 2) error = 1;
        }


        if(error){
            printf("%s%s", KRED, "Unrecognized command !\n");
            error = 0;
        }
        pwd(1);
    }
}

