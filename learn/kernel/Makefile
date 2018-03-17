KERNEL_DIR:=/usr/src/linux-headers-4.12.10-041210-generic
obj-m:=hello.o

default:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	$(RM)  *.cmd *.mod *.o *ko -r .tmp
