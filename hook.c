//
// Created by black on 22-10-13.
//

#include "hook.h"
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/unwind.h>
#include <linux/version.h>

// 用户空间的寄存器会以pt_regs结构体的形式，存储在当前内核栈空间的最高地址处
// 获取用户线程原本的寄存器保存位置
struct pt_regs *GetUserRegisters(struct task_struct *task) {
    struct unwind_state state;
    task = task ?: current;
    unwind_start(&state, task, NULL, NULL);
    return (struct pt_regs *) (((size_t) state.stack_info.end) - sizeof(struct pt_regs));
}


// x86_64 架构所有调用约定都必须从寄存器传参数，因此前面系统调用的参数就没法传到该函数中
void FtraceHandle(unsigned long ip, unsigned long parent_ip,
                  struct ftrace_ops *ops, struct pt_regs *regs) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
    int bit = ftrace_test_recursion_trylock(ip, parent_ip);
    if (bit < 0)
        return;
#endif

    struct pt_regs *user_regs = GetUserRegisters(NULL);
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);
    hook->function(user_regs);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
    ftrace_test_recursion_unlock(bit);
#endif
}

long HookMkdir(struct pt_regs *regs) {
    const char __user *pathname = (char *) regs->di;
    umode_t mode = regs->si;
    char buff[256];
    buff[255] = '\0';
    long res = strncpy_from_user(buff, pathname, 255);
    if (res)
        pr_info("[FtraceHandle] pathname: %s\n", buff);
    pr_info("[FtraceHandle] umode: %o\n", mode);
    return 0;
}

void FtraceHook(struct ftrace_ops *ops, unsigned char *orig_function_name) {
    int err;
    err = ftrace_set_filter(ops, orig_function_name, strlen(orig_function_name), 0);
    if (err) {
        pr_err("[FTraceHook] ftrace_set_filter() failed: %d\n", err);
        return;
    }
    err = register_ftrace_function(ops);
    if (err) {
        pr_err("[FTraceHook] register_ftrace_function() failed: %d\n", err);
        return;
    }
}

void FtraceUnHook(struct ftrace_ops *ops) {
    unregister_ftrace_function(ops);
    int err = ftrace_set_filter(ops, NULL, 0, 1);
    if (err) {
        pr_err("[FTraceHook] ftrace_reset_filter() failed: %d\n", err);
        return;
    }
}
