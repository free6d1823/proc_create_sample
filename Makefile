MODULE	:= tempdrv.ko
EXTRA_CFLAGS += -O -g -D_DEBUG

ifneq ($(KERNELRELEASE),)
obj-m := main.o
else
    PWD := $(shell pwd)
    KERNELDIR := $(KERNELSRC)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	@ rm -rf *.o *.ko

endif
