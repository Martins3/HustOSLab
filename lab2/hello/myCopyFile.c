/**
asmlinkage int sys_mycopy(char* src, char* dst){
    char buf[1024];
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    int rFile = sys_open(src, O_RDONLY, 0);
    if(rFile == -1) {
        printk("HuSYS: 打开读取文件错误\n");
        return -1;
    }

    int wFile = sys_open(dst, O_WRONLY | O_CREAT, 0666);
    if(wFile == -1) {
        printk("HuSYS: 打开写入文件错误\n");
        return -1;
    }

    int size;
    while((size = sys_read(rFile, buf, 1024)) > 0){
        sys_write(wFile, buf, size);
        printk("HuSYS: 写入文件大小的%d", size);
    }

    sys_close(wFile);
    sys_close(rFile);
    set_fs(old_fs);
    printk("HuSYS: 成功完成!\n");
    return 0;
}
*/
