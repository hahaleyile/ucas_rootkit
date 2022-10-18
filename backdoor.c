//
// Created by black on 22-10-12.
//

#include "backdoor.h"
#include <linux/kernel.h>
#include <linux/module.h>

// Optional
int open_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}

// Optional
int release_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}

static char command[2];

/*
** This function will be called when we read the procfs file
*/
ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    pr_info("proc file read.....\n");

    return length;;
}


/*
** This function will be called when we write the procfs file
*/
ssize_t write_proc(struct file *filp, const char __user *buff, size_t len, loff_t *off) {
    pr_info("proc file wrote.....\n");

    // buff 是 user mod 地址，需要拷贝到内核中
    // 不然会造成 exc_page_fault 错误
    if (!access_ok(buff, 2) || copy_from_user(command, buff, 2))
        return len;

    switch (command[0]) {
        case '0':
            break;
        case '1':
            break;
        default:
            break;
    }

    return len;
}

