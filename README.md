# Linux kernel 5.10 rootkit

## 开发环境搭建
为了方便驱动模块调试，我们需要自己编译一个 Linux 内核。
Debian 是一个非常适合开发的发行版，不管是编译内核、安装内核还是调试它都很容易操作。

我们首先通过 `apt install linux-source-xxx` 命令安装我们需要的版本的内核，该内核适用于 Debian 发行版。

![](/images/Pasted%20image%2020221020163056.png)

然后我们再通过 `make menuconfig` 选择我们要的选项：

![](/images/Pasted%20image%2020221020163145.png)

选择完成后使用 `make deb-pkg` 命令而不是简单的 `make` 命令，因为这样会产生我们要的 4 个 deb 安装包，方便在后续安装内核、开发用的头文件和 调试用的带符号信息的内核文件。

![](/images/Pasted%20image%2020221020163253.png)

调试开发环境选用 VMware 的双机环境，做好快照，方便系统崩溃后回滚，选用双机环境的原因是开发部署分离，不会污染开发环境。

![](/images/Pasted%20image%2020221020163322.png)

等待内核编译好之后就可以使用 `dpkg` 命令在部署机上安装内核了，同时在开发机上安装头文件和带调试信息的文件。

如果需要调试模块初始化函数和模块结束函数，则需要修改 gdb 脚本，修改下面位置。

添加代码的部分表示在安装模块的时候让 gdb 多读取模块的 `.init.text` 和 `.exit.text` 节的符号信息，这样安装之前下的断点就能够断下来。

![修改脚本前](/images/Pasted%20image%2020221020164032.png)

![修改脚本后](/images/Pasted%20image%2020221020163525.png)

最后配置双机调试的方法，既可以选择传统的串口调试方法，也可以使用 `kgdboe` 项目，一个基于网络的 kgdb 远程调试方法，不过我更推荐使用 VMware 自带的 debug stub，配置更简单，更不容易崩溃。

![](/images/Pasted%20image%2020221020164117.png)

## 项目结构

模块的入口点位于 `main.c` ，初始化函数只完成函数挂钩操作，退出函数则完成解钩和链表释放操作。
软件的核心操作是挂钩操作，核心数据结构是链表，这两者分别位于 hook 和 data 文件中。

hook 的方法选用 ftrace hook，通过 `ftrace_set_filter` 指定需要挂钩的函数，通过 `register_ftrace_function` 注册上自己写好的挂钩函数。
和一般的方法不同，我这边的 ftrace hook 系统调用时一般不需要修改 ip 寄存器，因为 ftrace 的原理就是将原函数的头 5 个字节写为 call 指令，所以挂钩函数正常退出后应该也能回到 call 下一条的地址。
事实也确实如此。
不同于 x86_32，x86_64 没有一种调用约定能从栈中读取参数，而 ftrace 在 hook 系统调用时只会将需要的参数压入栈中，而传入的参数是从寄存器里来的，是没用的（hook 其他内核函数时情况又有所不同）。
所以，参数的获取途径只能另寻他法。
好在调用系统调用时用户会将调用的参数压入栈中，这时我们可以读取用户栈来获取到参数。
另外说一句，asmlinkage 只对 32 位系统有用，64 位系统下它的宏定义为空。

我设计的 ftrace hook 很灵和，既可以不改 ip 返回原函数，也可以改 ip 跳过原函数，判断逻辑就是挂钩函数的返回值是否为 0。
同时，如果改 ip 跳过原函数，则下一条的地址需要通过全局变量 `jump_func` 进行指定。

数据结构采用了内核定义好的链表，因为它是非常可靠且线程安全的。
利用 `contains_of` 宏的机制，我们就可以任意定义链表结构内的成员。

后门设计为 hook `/dev/zero` 设备，因为 hook 现有设备不容易留下痕迹。
通过写入设备的前缀是否为 `ucashomework@` 来判断是不是我们要的指令，如果不是的话转交给原设备处理。
由于绝大多数写入的字符串都不是后门指令，因此这里我用了一个 `likely`，可以帮助编译器做分支优化。
所有后门指令的前缀可见下图。

![](/images/Pasted%20image%2020221020190731.png)

模块隐藏的原理是删除内核链表中的项，恢复原理逻辑相反。

文件隐藏（进程也是文件的一种）采用 hook `getdents64` ，通过 inode 和文件名两者同时相同来判断该文件是否需要隐藏。
如果需要隐藏，删掉传给用户的结构体中要隐藏的部分。
添加所需的文件需要指定绝对路径，然后程序会调用 `kern_path` 来判断文件是否存在，如果存在则将文件名和 inode 添加到链表中。
删除文件隐藏只需要指定文件名即可。
需要注意的是，进程隐藏还应该 hook kill 系统调用，因为 0 号信号可以用来判断进程是否存在，所幸我写的 hook 能够修改被挂钩函数的返回值。

端口隐藏我们只做了 tcp4 的隐藏，通过 hook `tcp4_seq_show` 函数，判断该端口需要隐藏后，直接返回 0。

进程保护就是保护进程不被杀，我们 hook 了 `kill` 函数，如果 pid 与我们要保护的 pid 相同，我们就直接返回 `-ESRCH`。


## 参考资料

* Makefile, Kbuild 请参考这些链接 [Linux Kernel Makefiles](https://docs.kernel.org/kbuild/makefiles.html) [Building External Modules](https://docs.kernel.org/kbuild/modules.html)
* ftrace hook [Using ftrace to hook to functions](https://01.org/linuxgraphics/gfx-docs/drm/trace/ftrace-uses.html)
* [Linux Syscall Reference ](https://syscalls64.paolostivanin.com/)