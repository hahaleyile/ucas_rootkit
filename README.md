# ucas_rootkit

## 新增rootkit模块

- 在`project/src`和`project/include`添加源代码与头文件
  - 头文件注意进行防御性声明，防止重复引用
    ```
    #ifndef XXX
    #define XXX

    #endif
    ```
- 将源代码的名称添加进`project/Makefile`中的`rootkit-y`变量
- 新增模块，需要定义自己的初始化函数与销毁函数。
  - 在初始化函数中，对模块所使用的资源进行初始化，完成内核函数的Hook工作。
  - 在销毁函数中，释放模块所使用的资源，取消内核函数的hook。
 - 在`src/Main.c`中添加对新模块初始化/销毁函数的调用
    ```
    static int __init rootkit_init(void) {
        initHiddenFile(); // 模块的初始化函数调用
        return 0;
    }

    static void __exit rootkit_exit(void) {
        finiHiddenFile(); // 模块的销毁函数调用
    }
    ```

## 关于Hook内核函数

引用了[ftrace_helper](https://github.com/xcellerator/linux_kernel_hacking/blob/master/3_RootkitTechniques/3.4_hiding_directories/ftrace_helper.h)
