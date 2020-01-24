obj-m := bluerandomModule.o
#module-objs := hci_core.o bluetooth.o

PWD := $(shell pwd)
#KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /lib/modules/4.8.0-53-generic/build
#KDIR := /usr/src/linux-source-4.4.0/linux-source-4.4.0/
KDIR := /usr/src/linux-source-4.8.0/linux-source-4.8.0/

#KBUILD_EXTRA_SYMBOLS := /lib/modules/4.8.0-53-generic/kernel/net/bluetooth/Module.symvers

#KBUILD_EXTRA_SYMBOLS := linux-hwe-4.15.0/include/net/bluetooth/

all:
	$(MAKE) -C $(KDIR) M=$(PWD)  modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
