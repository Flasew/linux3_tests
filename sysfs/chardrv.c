#include "chardrv.h"

MODULE_LICENSE("Dual BSD/GPL");


atomic_t the_value;

static int __init char_init(void) {
	
	
	alloc_chrdev_region(&devno,0,1,"testchar");
	
	cdev_init(&cdev, &fops);
	cdev_add(&cdev, devno,1);
	
	sysfs_class = class_create(THIS_MODULE,"chardrv");
	sysfs_device = device_create(sysfs_class,NULL,devno,NULL,"chardrv");
	device_create_file(sysfs_device,&dev_attr_some_attr);
	
	
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
	return 0;

}

static int driver_release(struct inode *inodep, struct file *filep) {
	return 0;
}

static int driver_open(struct inode *inodep, struct file *filep) {
	return 0;
}

static ssize_t some_attr_show(struct device * dev, struct device_attribute * attr, char * buf){

	return snprintf((char*)buf,PAGE_SIZE,"%d\n",the_value.counter);

	
}

static ssize_t some_attr_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t count){
	int tmp;
	sscanf(buf,"%d",&tmp);
	atomic_set(&the_value,tmp);
	return PAGE_SIZE;
}





module_init(char_init);
module_exit(char_exit);
