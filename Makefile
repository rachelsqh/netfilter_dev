obj-m := netfilter_dev.o
KDIR :=/lib/modules/`uname -r`/build
PWD := `pwd`
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
