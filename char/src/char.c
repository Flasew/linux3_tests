#include "char.h"

MODULE_LICENSE("Dual BSD/GPL");



static int __init char_init(void) {

	devno = MKDEV(major,minor);
	cdev = cdev_alloc();
	cdev_init(cdev,&f_ops);
	cdev_add(cdev,devno,1);
	register_chrdev_region(devno,1,name);
	
	mutex_init(&lock);
	
	printk(KERN_ALERT "[char] irq = %d, target=%s\n", irq, target_dev);

	return 0;
}

static void __exit char_exit(void) {

	unregister_chrdev_region(devno,1);
	cdev_del(cdev);
	
	mutex_destroy(&lock);

	printk(KERN_ALERT "[char] unregistered\n");
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
	
	mutex_unlock(&lock);
	
	
	return 0;
	
}

static int dev_open(struct inode *inodep, struct file *filep) {

	if(!mutex_trylock(&lock)) {return -EBUSY;}


	return 0;
	
}


module_init(char_init);
module_exit(char_exit);
