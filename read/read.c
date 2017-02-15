#include "read.h"

MODULE_LICENSE("Dual BSD/GPL");

atomic_t irqcount = ATOMIC_INIT(0);

dev_t devno;
struct cdev cdev;

struct class * sysfs_class;
struct device * sysfs_device;

struct mutex readlock;
struct mutex writelock;
struct mutex driverlock;

struct class * sysfs_class;
struct device * sysfs_device;

/* somehow, this still requires root to chmod the sysfs entry for write access */
DEVICE_ATTR(some_attr, S_IRWXU |
	S_IRWXG |
	S_IRGRP |
	S_IWGRP ,
	some_attr_show,
	some_attr_store);


static int driver_open(struct inode *inodep, struct file *filep){
	if(!mutex_trylock(&driverlock)) {return -EBUSY;}
	printk(KERN_ALERT "[read] open\n");
	return 0;
}

static int driver_release(struct inode *inodep, struct file *filep){
	mutex_unlock(&driverlock);
	printk(KERN_ALERT "[read] close\n");
	return 0;
}

static int __init read_init(void) {

	/* char dev and udev setup */

	alloc_chrdev_region(&devno,0,1,"read");
	
	cdev_init(&cdev, &fops);
	cdev_add(&cdev, devno,1);
	
	sysfs_class = class_create(THIS_MODULE,"read");
	sysfs_device = device_create(sysfs_class,NULL,devno,NULL,"read");
	
	/* register an interrupt handler */
	
	request_irq(
		interrupt_line,
		(irq_handler_t) isr,
		interrupt_flags,
		"read",
		(void*) &interrupt_flags
	);
	
	/* initialize mutexes */
	
	mutex_init(&driverlock);
	mutex_init(&readlock);
	mutex_init(&writelock);
	
	/* allocate read buffer */

}

static void __exit read_exit(void) {

	/* char dev and udev exit */

	device_destroy(sysfs_class,devno);
	class_destroy(sysfs_class);

	unregister_chrdev_region(devno,1);
	cdev_del(&cdev);	
	
	/* free irq */

	free_irq(interrupt_line, (void *) &interrupt_flags);

	/* destroy mutexes */
	
	mutex_destroy(&driverlock);
	mutex_destroy(&readlock);
	mutex_destroy(&writelock);

	printk(KERN_ALERT "[read] unregistered\n");
}



irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {


}


/*void msg_dispatch(struct work_struct * work){}*/

module_init(read_init);
module_exit(read_exit);
