obj-m := poison_test.o

ccflags-y := -DDEBUG -Wformat=0

KERNELDIR ?= /work/metax/linux_v5.4/
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build


PWD := $(shell pwd)
default:
	$(MAKE) -C  $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

