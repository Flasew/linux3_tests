#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static char* target_dev;
static int irq = 0;
static int irqflags = 0x0080;
static int major = 0;

module_param(irq, int, 0644);
module_param(irqflags, int, 0644);
module_param(major, int, 0644);
module_param(target_dev, charp, 0644);


static int __init char_init(void) {

	printk(KERN_ALERT "[char] irq = %d, target=%s\n", irq, target_dev);

	return 0;
}

static void __exit char_exit(void) {

	printk(KERN_ALERT "[char] unregistered\n");
}

module_init(char_init);
module_exit(char_exit);
