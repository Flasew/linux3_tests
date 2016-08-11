obj-m := full2.o
full2-objs := ./src/full2.o ./src/vfs.o

PWD := `pwd`
KDIR := /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
