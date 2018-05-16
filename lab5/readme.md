1. 到底什么时候写入 inode 的消息, 绝对不可以发生 缺失 ！

2. 首先完成一份的文本界面的版本， 然后在Qt 封装
    1. 打开文件默认为 w+ 模式
    2. 修改的文件名的位置
    3. debug 函数
        1. 暂时测试部分为写入不超过一个块大小的部分

3. 绘制Qt 界面

config:
1. 相同目录 dir 和 file 不可以名字相同的
2. 


debug: 
1. 担心 读写位置的含有错误， 出现了的把 磁盘数据修改的情况的
2. 含有数据刷新的问题, 如果管理 curDir, 和 root_node 数据
应该的清除掉root_inode 使用的cur_inode 指针管理的所有的数据的
问题是反复的查询root_inode , 
添加函数 用于防止当当前的目录就是的， 
root_inode 最后存盘


3. 注意存盘

4. read 的 fseek 需要实在文件结束的位置
5. inode 的大小 有问题的  ？
6. 任何文件指针的管理 ！=》 dir , 使用之前设置为0
7. root_node 指向第一个块是 204799 吗 ？

1. 向回写的时候含有问题 ！

1. 如果读取block 正确与否的时候 都是会出现的正确结果 ！

为什么 含有 inode 
1. 初始化的时候， 
2. 如果的进入到目录， 那么写入如果不是root, 写入数据， 修改指针， 

1. cd 的时候 可以进入的到文件的中间去
2. 删除文件的时候 会出现的

发现唯独的root_inode cons 出现问题
```
    fclose(fptr);

    fptr = fopen("/home/martin/X-Brain/Notes/Atom/os/project/fs/log", "rb+");
    char stupid[1024 * 1024];
    for(int i = 0; i < 1; i++)
        fwrite(stupid, sizeof(char), sizeof(stupid), fptr);

    fFile fp = get_cur_dir();
    fp->next_free = 12345;
    print_inode(fp);
    fseek(fptr, 0, SEEK_SET);
    fwrite(fp, sizeof(INODE), 1, fptr);
    
    fp->next_free = 2;
    fseek(fptr, sizeof(INODE), SEEK_SET);
    fwrite(fp, sizeof(INODE), 1, fptr);



    printf("%lu", ftell(fptr));
    fseek(fptr, 0, SEEK_SET);
    fread(fp, sizeof(INODE), 1, fptr);
    print_inode(fp);

    fseek(fptr, sizeof(INODE) , SEEK_SET);
    fread(fp, sizeof(INODE), 1, fptr);
    print_inode(fp);
```