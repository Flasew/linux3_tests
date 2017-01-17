#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h> 

#include <linux/device.h>

#include <linux/sysfs.h>



dev_t devno;
struct cdev cdev;

struct class * sysfs_class;
struct device * sysfs_device;

static int driver_open(struct inode *inodep, struct file *filep);
static int driver_release(struct inode *inodep, struct file *filep);




static ssize_t driver_write(struct file *filep,	const char *buffer,	size_t len,	loff_t *offset);

static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write,
	.release = driver_release,
};


static ssize_t some_attr_show(struct device * dev, struct device_attribute * attr, char * buf);

static ssize_t some_attr_store(struct device * dev, struct device_attribute * attr, char * buf, size_t count);


DEVICE_ATTR(some_attr, S_IRUSR | S_IWUSR, some_attr_show, some_attr_store);
