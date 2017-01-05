obj-m := char.o
char-objs := ./src/char.o ./src/vfs.o

PWD := `pwd`
KDIR := /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
