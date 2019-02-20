# 架构
application ： 命令解析 和 执行命令
io : 打开 删除 读 写
kernel: 100M文件的初始化 运行的加载super\_block节点 磁盘块的释放和分配

依托于inode节点 建立树形状结构
通过多级磁盘， 实现整个文件的管理


## 存储的分布
config
成组链接法 的 super\_node 启动的时候加载进入
inode节点 数组
磁盘

## 配置
1. 磁盘编号为顺序编号

## 成组链接法
1. 初始化100M文件的时候， 逐个回收文件
2. 释放 和 回收
    数值 s = 100
    数组 k[100]

## 链表管理free inode

## 如何实现删除的文件或者文件夹
删除文件:
    1. 删除所在节点的表项
    2. 删除inode
    3. 释放控制的磁盘块

删除文件夹: 递归删除


## 如何实现读写文件
1. 长度超过缓冲区的大小
2. 删除文件之后， 如何回收磁盘块

## inode中间含有什么
文件基本信息
inode管理
多级文件的管理

## 如何集成vim的
```
    printf("start vim !\n");
    char * args[] = {"vim", VIM_BUF_PATH, NULL};
    int status;
    if(!fork()){
        execvp("vim", args);
    }
    wait(&status);
```

> The exec() family of functions replaces the current process image with a new process image.

## 如何解析命令
解析成为数组， 然后逐个解析

## 支持那些命令
增删改查 + vim

## 如何读写100M的文件
创建使用 a+
读写使用 rb+
