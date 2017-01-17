#include "char.h"

MODULE_LICENSE("Dual BSD/GPL");

static int __init char_init(void) {
	
	
	alloc_chrdev_region(&devno,0,1,"testchar");
	
	cdev_init(&cdev, &fops);
	cdev_add(&cdev, params.devno,1);
	
	sysfs_class = class_create(THIS_MODULE,"ctestchar");
	sysfs_device = device_create(sysfs_class,NULL,devno,NULL,"testchar");
	device_create_file(sysfs_device,&some_attr);
	
	
	printk(KERN_ALERT "[char] registered\n");

	return 0;
}

static void __exit char_exit(void) {


	

	device_destroy(sysfs_class,devno);
	class_destroy(sysfs_class);

	unregister_chrdev_region(devno,1);
	cdev_del(&cdev);

	printk(KERN_ALERT "[char] unregistered\n");
}

static ssize_t driver_write(struct file *filep,
	const char *buffer,
	size_t len,
	loff_t *offset){
		
	printk(KERN_ALERT "[char] write");
	

}

static int driver_release(struct inode *inodep, struct file *filep) {
	return 0;
}

static int driver_open(struct inode *inodep, struct file *filep) {
	return 0;
}











static ssize_t some_attr_show(struct device * dev, struct device_attribute * attr, char * buf);

static ssize_t some_attr_store(struct device * dev, struct device_attribute * attr, char * buf, size_t count);





module_init(char_init);
module_exit(char_exit);
