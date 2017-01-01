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

#include <linux/wait.h>
#include <delay.h>

#include "vfs.h"

/* Neither of these structs are intended to be dynamically
 * initializable. They're just meant as namespaces to organize
 * an unwieldy number of globals. */

struct driver_params {
	
	char* target_path;
	char* driver_name;
	int irqno;
	int irqflags;
	
	int delay_msec;
	int delay_nsec;
	
	int major;
	dev_t devno;
	struct cdev cdev;
	struct file_operations fops;
	
};


struct driver_state {
	
	int irqcount;
	
	struct file* f;
	
	struct workqueue_struct* workqueue;
	struct work_struct work;
	
	struct wait_queue_head_t waitqueue;
	
	struct mutex lock;
	

};

int driver_open(struct inode *, struct file *);
int driver_release(struct inode *, struct file *);
ssize_t driver_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = dev_open,
	.write = dev_write,
	.release = dev_release,
};


static struct driver_params params;
static struct driver_state state ;


module_param(params.irq, int, 0644);
module_param(params.irqflags, int, 0644);
module_param(params.major, int, 0644);
module_param(params.delay_msec, int, 0644);
module_param(params.delay_nsec, int, 0644);
module_param(params.driver_name, charp, 0644);
module_param(params.target_path, charp, 0644);
