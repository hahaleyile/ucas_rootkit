//
// Created by black on 22-10-16.
//

#ifndef UCAS_ROOTKIT_FILE_H
#define UCAS_ROOTKIT_FILE_H

#include <linux/ftrace.h>

long HookGetdents64(struct pt_regs *regs, long (*orig_func)(const struct pt_regs *));

#endif //UCAS_ROOTKIT_FILE_H
