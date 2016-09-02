obj-m := vfs_wq.o
vfs_wq-objs := ./src/vfs.o

PWD := `pwd`
KDIR := /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
