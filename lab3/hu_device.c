#include <linux/init.h>        
#include <linux/module.h>
#include <linux/device.h>        
#include <linux/kernel.h>        
#include <linux/fs.h>            
#include <linux/uaccess.h>   


#define  DEVICE_NAME "hu_device"   
#define  CLASS_NAME  "xueshi"       
#define MSG_SIZE 1024

MODULE_LICENSE("GPL");           
MODULE_AUTHOR("Xue shi Hu");    
MODULE_DESCRIPTION("the lab two"); 
MODULE_VERSION("0.1");            

static int    majorNumber;                  
static int    write_count;                  
static char   num_one[MSG_SIZE] = {0};          
static char   num_two[MSG_SIZE] = {0};          
static char   num_res[MSG_SIZE] = {0};          
static char   error_msg[MSG_SIZE] = "need more input\n";          

static struct class*  xueshi_char_Class  = NULL; 
static struct device* xueshi_char_Device = NULL; 


static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};


static int __init xueshi_char_init(void){
    // 注册设备
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0){
       return majorNumber;
    }
    printk(KERN_INFO "Hu_CHAR_DEV: 注册成功 %d\n", majorNumber);

    //注册设备类
    xueshi_char_Class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(xueshi_char_Class)){                
       unregister_chrdev(majorNumber, DEVICE_NAME);
       return PTR_ERR(xueshi_char_Class);          
    }
    printk(KERN_INFO "Hu_CHAR_DEV: 设备类注册成功\n");

    xueshi_char_Device = device_create(xueshi_char_Class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(xueshi_char_Device)){               
       class_destroy(xueshi_char_Class);           
       unregister_chrdev(majorNumber, DEVICE_NAME);
       return PTR_ERR(xueshi_char_Device);
    }
    
    printk(KERN_INFO "Hu_CHAR_DEV: 设备驱动注册成功\n"); 
    printk(KERN_INFO "Hu_CHAR_DEV: 设备初始化\n");
    return 0;
}


static void __exit xueshi_char_exit(void){
   device_destroy(xueshi_char_Class, MKDEV(majorNumber, 0));    
   class_unregister(xueshi_char_Class);                          
   class_destroy(xueshi_char_Class);                             
   unregister_chrdev(majorNumber, DEVICE_NAME);             
   printk(KERN_INFO "Hu_CHAR_DEV: 设备注销成功!\n");
}


static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Hu_CHAR_DEV: 打开设备!\n");
   return 0;
}


/**
 * 当出现两次的写 之后 数据 可以 读, 否则返回等待的输入的结果
 * 读取数据总是从开始的位置读入数据 offset 没有意义
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    char * who;
    if(write_count != 2){
        who = error_msg; 
    }else{
        who = num_res;
        write_count = 0;
    }

    if (copy_to_user(buffer, (void*)(who), len)){
        printk(KERN_INFO "Hu_CHAR_DEV: read failed\n");
        return -1;
    }else{
        who[MSG_SIZE - 1] = 0;
        printk(KERN_INFO "Hu_CHAR_DEV: %s\n", who);
    }

    return 1;

}

// 用户写
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    char * who;
    if(write_count == 0){
        who = num_one;
    }else if(write_count == 1){
        who = num_two;
    }else{
        return -1;
    }
    write_count ++;

    if (copy_from_user((void*)(who), buffer, len)){
        printk(KERN_INFO "Hu_CHAR_DEV: read failed\n");
        return -1;
    }else{
        if(write_count == 2){
            int i = 0;
            int j = 0;
            int s = MSG_SIZE;
            who[MSG_SIZE - 1] = 0; // 防卫型代码            
            printk(KERN_INFO "Hu_CHAR_DEV: %s\n", who);
            // 计算的数据 放置到的res 中间的去
            while(true){
                int k = i > j ? i : j;
                num_res[k] = (num_one[i] + num_two[j] - '0') % 16 + '0';

                if(num_one[i] != '\0') i++;
                if(num_two[j] != '\0') j++;

                if(!s) break;
                s --;
                printk(KERN_INFO "Hu_CHAR_DEV: loop %d\n", s);
                if(num_one[i] == '\0' && num_two[j] == '\0') break; 
            }
        }
    }
    return 1; 
}

static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "Hu_CHAR_DEV: 设备关闭成功 ！\n");
    return 0;
}

module_init(xueshi_char_init);
module_exit(xueshi_char_exit);