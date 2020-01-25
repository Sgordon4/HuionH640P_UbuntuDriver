obj-m := dp_driver.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)


#.PHONY: all

all:
	# M/SUBDIRS sets the directory to look for the kernel source when building external modules.
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *- *.mk *.cmd
