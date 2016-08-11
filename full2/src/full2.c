#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/interrupt.h>

#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/ktime.h>

#include "vfs.h"


#define IRQNUMBER 1

MODULE_LICENSE("Dual BSD/GPL");

struct driver_state {
	int irq_count;
	ktime_t t_irq;
	struct file* f;
	struct workqueue_struct* wq;
	struct work_struct ws;
	char* dev_id;
};

static struct driver_state state = {
	.irq_count = 0,
	.dev_id = "full2_test"
};

void test(struct work_struct* work){

	static ktime_t buff[3];

	buff[0] = state.t_irq;
	buff[1] = ktime_get();
	
	file_write(state.f, (24*state.irq_count), (unsigned char *) buff, 24);

	buff[2] = ktime_get();
	
	file_sync(state.f);
	state.irq_count += 1;
}

irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	state.t_irq = ktime_get();
	queue_work(state.wq,&state.ws);
	return (irq_handler_t) IRQ_HANDLED;
}

static int __init full2_init(void) {

	int ok;

	ok = request_irq(IRQNUMBER,
		(irq_handler_t) isr,
		0x0080, /* 0x2080 IRQF_SHARED | IRQF_ONESHOT */
		"full2",
		(void*)state.dev_id);

	INIT_WORK(&state.ws, test);

	state.wq = create_workqueue("wq");
	state.f = file_open("full2_test.out", O_WRONLY | O_CREAT, S_IRWXO);

	printk(KERN_ALERT "[full] request_irq: %d | create_workqueue: %pB", ok,state.wq);
	printk(KERN_ALERT "[full] registered\n");

	return 0;
}

static void __exit full2_exit(void) {
	flush_workqueue(state.wq);
	destroy_workqueue(state.wq);
	file_close(state.f);
	free_irq(IRQNUMBER,(void*) state.dev_id);
	printk(KERN_ALERT "[full] unregistered\n");
}

module_init(full2_init);
module_exit(full2_exit);
