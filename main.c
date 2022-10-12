#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "backdoor.h"

static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};


static int __init hello_init(void) {
    pr_info("hello!\n");

    proc_create("test", 0666, NULL, &proc_fops);

    return 0;
}

static void __exit hello_exit(void) {
    pr_info("hello_exit!\n");
    remove_proc_entry("test", NULL);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
