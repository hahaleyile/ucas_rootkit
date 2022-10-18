//
// Created by black on 22-10-17.
//

#include "process.h"
#include "data.h"
#include <linux/kernel.h>

long HookKill(struct pt_regs *regs, long (*orig_func)(const struct pt_regs *)) {
    pid_t pid = regs->di;
    int sig = regs->si;

    struct list_head *pos;
    struct List *curr;
    unsigned int list_pid;
    int err;
    list_for_each(pos, &protect_process_list) {
        curr = list_entry_rcu(pos, struct List, list);
        err = kstrtouint(curr->data, 10, &list_pid);
        if (err) {
            pr_info("Protect process list is broken!\n");
            return 0;
        }
        if (list_pid == pid) {
            pr_info("Protect process %d from signal %d.\n", pid, sig);
            // 指示不执行 kill 系统调用
            return -ESRCH;
        }
    }

    return 0;
}
