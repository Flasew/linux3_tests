#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h> 
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/string.h>
#include <linux/jiffies.h>

#include <linux/wait.h>
#include <delay.h>

#include "vfs.h"

/* Neither of these structs are intended to be dynamically
 * initializable. They're just meant as namespaces to organize
 * an unwieldy number of globals. */

struct driver_params {
	
	char* target_path;
	char* driver_name;
	unsigned int irqno;
	int irqflags;
	
	unsigned long delay_msec;
	unsigned long delay_usec;
	
	int timeout_msec;
	unsigned long timeout_jiffies;
	
	int major;
	dev_t devno;
	struct cdev cdev;
	struct file_operations fops;
	
};


struct driver_state {
	
	int irqcount;
		
	struct file* f; /* destination for vfs write */
	
	struct workqueue_struct* workqueue;
	struct work_struct work;
	
	wait_queue_head_t waitqueue;
	
	struct mutex lock; /* prevent multiple processes from using driver */
	
	char msg[256]; 
	int msg_len;
	
	int sent; /* number of bytes sent on last vfs write */
	
};

int driver_open(struct inode *, struct file *);
int driver_release(struct inode *, struct file *);

/* Latch a value to send to the target on the next interrupt cycle.
 * No value is sent on the next interrupt if not called.
 * Returns the number of bytes written on the LAST cycle.*/

ssize_t driver_write(struct file *,
	const char *,
	size_t, loff_t *);

/* Send the stored message, if any, to the target
 * device and wake the process */

irq_handler_t isr(unsigned int irq,
	void *dev_id,
	struct pt_regs *regs)

/* Worker function that the isr places in a workqueue 
 * to send message to target device via vfs_write.
 * Necessary because vfs_write can't run in an atomic
 * context.*/

void msg_dispatch(struct work_struct work);

static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = dev_open,
	.write = dev_write,
	.release = dev_release,
};


static struct driver_params params = {
	.fops = fops,
};

static struct driver_state state ;


module_param(params.irq, uint, 0644);
module_param(params.irqflags, int, 0644);
module_param(params.major, int, 0644);
module_param(params.timeout_msec, int, 0644);
module_param(params.delay_msec, ulong, 0644);
module_param(params.delay_usec, ulong, 0644);
module_param(params.driver_name, charp, 0644);
module_param(params.target_path, charp, 0644);
