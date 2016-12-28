#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/module.h>

#include <linux/fs.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("Dual BSD/GPL");

static char* target_dev;
static int irq = 0;
static int irqflags = 0x0080;
static int major = 0;

static int minor = 0;
static dev_t dev;

module_param(irq, int, 0644);
module_param(irqflags, int, 0644);
module_param(major, int, 0644);
module_param(target_dev, charp, 0644);


static int __init char_init(void) {

	dev = MKDEV(major,minor);

	if (!register_chrdev_region(dev,1,"acb")){
		printk(KERN_ALERT "[char] could not register character device");
	}


	printk(KERN_ALERT "[char] irq = %d, target=%s\n", irq, target_dev);

	return 0;
}

static void __exit char_exit(void) {

	unregister_chrdev_region(dev,1);

	printk(KERN_ALERT "[char] unregistered\n");
}

module_init(char_init);
module_exit(char_exit);
