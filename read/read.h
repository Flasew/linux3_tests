#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h> 
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/ktime.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

static int irqcount;

struct mutex readlock;
struct mutex writelock;
struct mutex driverlock;

dev_t devno;
struct cdev cdev;

struct class * sysfs_class;
struct device * sysfs_device;



static int driver_open(struct inode *inodep, struct file *filep);
static int driver_release(struct inode *inodep, struct file *filep);

static ssize_t driver_write(struct file *filep,	const char *buffer,	size_t len,	loff_t *offset);

static ssize_t driver_read(sruct file * filep, char * buf, size_t len, loff_t * offset);


static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write,
	.read = driver_read,
	.release = driver_release,
};