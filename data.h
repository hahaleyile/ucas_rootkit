//
// Created by black on 22-10-17.
//

#ifndef UCAS_ROOTKIT_DATA_H
#define UCAS_ROOTKIT_DATA_H

#include <linux/list.h>
#include <linux/slab.h>

struct List {
    const char *data;
    unsigned long inode_or_port;
    struct list_head list;
};

extern struct list_head filename_list;

extern struct list_head protect_process_list;

extern struct list_head port_list;

void AddEntry(struct List l, struct list_head *head);

void DelEntryWithInode(struct List l, struct list_head *head);

void DelEntryWithData(struct List l, struct list_head *head);

bool ListContainsDataAndInode(struct List l, struct list_head *head);

bool ListContainsInode(struct List l, struct list_head *head);

#endif //UCAS_ROOTKIT_DATA_H
