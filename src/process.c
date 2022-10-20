//
// Created by black on 22-10-17.
//

#include <linux/kernel.h>
#include "process.h"
#include "data.h"
#include "hook.h"

long HookKill(struct pt_regs *regs, long (*orig_func)(const struct pt_regs *)) {
    pid_t pid = regs->di;
    int sig = regs->si;

    struct list_head *pos;
    struct List *curr;
    struct List temp = {.inode_or_port=pid};
    if (ListContainsInode(temp, &protect_process_list)) {
        pr_info("Protect process %d from signal %d.\n", pid, sig);
        // 指示不执行 kill 系统调用
        jump_func = (unsigned long) CleanFunc;
        return -ESRCH;
    }

    return 0;
}
