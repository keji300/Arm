ifeq ($(KERNELRELEASE),) 
all:
	make -C /home/alinx/Desktop/kernel/linux-3.14 M=$(shell pwd)  
clean:
	rm *.o *.ko *.mod.c modules.order
else 
obj-m:=key_int_driver_fs4412.o

endif
