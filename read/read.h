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
#include <linux/vmalloc.h>


static int driver_open(struct inode *inodep, struct file *filep);
static int driver_release(struct inode *inodep, struct file *filep);
static ssize_t driver_write(struct file *filep,	const char *buffer,	size_t len,	loff_t *offset);
static ssize_t driver_read(sruct file * filep, char * buf, size_t len, loff_t * offset);

irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs);

/*void msg_dispatch(struct work_struct * work);*/

static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write,
	.read = driver_read,
	.release = driver_release,
};


unsigned int interrupt_line;
int interrupt_flags;

module_param_named(irq,interrupt_line, uint, 0644);
module_param_named(irqflags,interrupt_flags, int, 0644);