//
// Created by black on 22-10-17.
//

#include <linux/rculist.h>
#include "data.h"

LIST_HEAD(filename_list);

LIST_HEAD(protect_process_list);

LIST_HEAD(port_list);

void AddEntry(struct List l, struct list_head *head) {
    struct List *temp;
    temp = (struct List *) kmalloc(sizeof(struct List), GFP_KERNEL);
    temp->data = kmalloc(sizeof(char) * (strlen(l.data) + 1), GFP_KERNEL);
    strcpy(temp->data, l.data);
    temp->inode_or_port = l.inode_or_port;
    list_add_tail_rcu(&(temp->list), head);
}

void DelEntryWithInode(struct List l, struct list_head *head) {
    struct list_head *curr, *next;
    struct List *entry;
    list_for_each_safe(curr, next, head) {
        entry = list_entry_rcu(curr, struct List, list);
        if (entry->inode_or_port == l.inode_or_port) {
            list_del_rcu(curr);
            kfree(entry->data);
            kfree(entry);
            return;
        }
    }
}


void DelEntryWithData(struct List l, struct list_head *head) {
    struct list_head *curr, *next;
    struct List *entry;
    list_for_each_safe(curr, next, head) {
        entry = list_entry_rcu(curr, struct List, list);
        if (!strncmp(l.data, entry->data, strlen(entry->data))) {
            list_del_rcu(curr);
            kfree(entry->data);
            kfree(entry);
            return;
        }
    }
}

bool ListContainsDataAndInode(struct List l, struct list_head *head) {
    struct list_head *curr, *next;
    struct List *entry;
    list_for_each_safe(curr, next, head) {
        entry = list_entry_rcu(curr, struct List, list);
        if (!strcmp(l.data, entry->data) && entry->inode_or_port == l.inode_or_port) {
            return true;
        }
    }
    return false;
}

bool ListContainsInode(struct List l, struct list_head *head) {
    struct list_head *curr, *next;
    struct List *entry;
    list_for_each_safe(curr, next, head) {
        entry = list_entry_rcu(curr, struct List, list);
        if (entry->inode_or_port == l.inode_or_port) {
            return true;
        }
    }
    return false;
}
