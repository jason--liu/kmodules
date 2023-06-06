ifneq ($(KERNELRELEASE),)
	obj-m := kthread.o
else

KERNELDIR ?= /work/metax/linux_v5.4/
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
EXTRA_CFLAGS += -DDEBUG -Wformat=0


PWD := $(shell pwd)
default:
	$(MAKE) -C  $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
 endif

