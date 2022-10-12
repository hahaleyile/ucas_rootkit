//
// Created by black on 22-10-12.
//

#ifndef UCAS_ROOTKIT_BACKDOOR_H
#define UCAS_ROOTKIT_BACKDOOR_H

#include <linux/proc_fs.h>

int open_proc(struct inode *inode, struct file *file);

int release_proc(struct inode *inode, struct file *file);

ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t *offset);

ssize_t write_proc(struct file *filp, const char __user *buff, size_t len, loff_t *off);

#endif //UCAS_ROOTKIT_BACKDOOR_H
