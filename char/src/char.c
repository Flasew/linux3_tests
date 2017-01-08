#include "char.h"

MODULE_LICENSE("Dual BSD/GPL");

static int __init char_init(void) {
	
	state.msg_len = 0;
	state.sent = 0;
	
	params.timeout_jiffies = 1000 * HZ * params.timeout_msec;
	
	params.devno = MKDEV(params.major,0);
	cdev_init(&params.cdev, &fops);
	cdev_add(&params.cdev, params.devno,1);
	
	register_chrdev_region(params.devno,
		1, params.driver_name);


	INIT_WORK(&state.work, msg_dispatch);

	state.workqueue = 
		create_workqueue(params.driver_name);
		
	state.f = file_open(params.target_path,
		O_WRONLY | O_CREAT,
		S_IRWXO);
		
		
	request_irq(params.irqno,
		(irq_handler_t) isr,
		params.irqflags,
		params.driver_name,
		(void*) &params);

	init_waitqueue_head(&state.waitqueue);

	mutex_init(&state.lock);
	state.irqcount = 0;
			
	printk(KERN_ALERT "[char] irq = %d, target=%s, major=%i\n",
		params.irqno,
		params.target_path,
		params.major);

	return 0;
}

static void __exit char_exit(void) {

	unregister_chrdev_region(params.devno,1);
	cdev_del(&params.cdev);
	
	mutex_destroy(&state.lock);
	
	flush_workqueue(state.workqueue);
	destroy_workqueue(state.workqueue);
	
	file_close(state.f);
	free_irq(params.irqno, (void *) &params);

	printk(KERN_ALERT "[char] unregistered\n");
}

static ssize_t driver_write(struct file *filep,
	const char *buffer,
	size_t len,
	loff_t *offset){	
	
	/* block until the last msg_dispatch() returns */
	
	printk(KERN_ALERT "[char] write\n");

	flush_workqueue(state.workqueue);	
	
	printk(KERN_ALERT "[char] write: flushed workqueue\n");
	
	state.msg_len = len;
	strncpy(state.msg,buffer,MAX_MSG_LEN);
	
	printk(KERN_ALERT "[char] write: sleep\n");
	
	wait_event_interruptible_timeout(state.waitqueue,
		state.sent != 0,
		params.timeout_jiffies);
		
	printk(KERN_ALERT "[char] write: wake\n");
	
	return state.sent;
}

static int driver_release(struct inode *inodep, struct file *filep) {
	
	state.msg_len = 0;
	state.sent = 0;
	
	flush_workqueue(state.workqueue);

	mutex_unlock(&state.lock);
	
	printk(KERN_ALERT "[char] close\n");

	
	return 0;
	
}

static int driver_open(struct inode *inodep, struct file *filep) {

	if(!mutex_trylock(&state.lock)) {return -EBUSY;}
	
	state.msg_len = 0;
	state.sent = 0;


	printk(KERN_ALERT "[char] open\n");

	return 0;
	
}



irq_handler_t isr(unsigned int irq,
	void *dev_id,
	struct pt_regs *regs){
		
	state.sent = 0;
	state.irqcount++;
	
	printk(KERN_ALERT "[char] isr\n");
	
		
	if (state.msg_len) {
		
		if (params.delay_msec) {mdelay(params.delay_msec);}
		if (params.delay_usec) {udelay(params.delay_usec);}
		
		/* can't do a vfs write directly */
		queue_work(state.workqueue,&state.work);
		
	}
	
	
	
	wake_up_interruptible(&state.waitqueue);
	
	printk(KERN_ALERT "[char] isr: woke user\n");
	
	return (irq_handler_t) IRQ_HANDLED;
	
}

void msg_dispatch(struct work_struct * work){
	
		printk(KERN_ALERT "[char] dispatch\n");
	
		/* TODO: What happens if this call hangs at all? */
	
		state.sent = file_write(state.f,0,
			state.msg,
			(unsigned int)state.msg_len);
			
		file_sync(state.f);
		
		state.msg_len = 0;
		
		printk(KERN_ALERT "[char] dispatch: filesync\n");
		
}

module_init(char_init);
module_exit(char_exit);
