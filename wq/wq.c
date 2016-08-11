#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#include <linux/ktime.h>

MODULE_LICENSE("Dual BSD/GPL");

#define IRQNUMBER 40





void test(void* args){
	printk(KERN_ALERT "[wq] workqueue ran\n");
}

static DECLARE_WORK(ws, test);
static struct workqueue_struct* wq;

irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	queue_work(wq,&ws);
	return (irq_handler_t) IRQ_HANDLED;
}

static int ok;

static int __init wq_init(void) {

	/*free_irq(IRQNUMBER,NULL);*/

	ok = request_irq(IRQNUMBER,
		(irq_handler_t) isr,
		0x2080, /* IRQF_SHARED | IRQF_ONESHOT */
		"wq_test",
		IRQF_NO_SUSPEND);

	wq = create_workqueue("wq");

	printk(KERN_ALERT "[wq] request_irq: %d | create_workqueue: %d", ok,wq);

	printk(KERN_ALERT "[wq] registered\n");

	return 0;
}

static void __exit wq_exit(void) {
	/*free_irq(IRQNUMBER,NULL);*/
	flush_workqueue(wq);
	destroy_workqueue(wq);
	printk(KERN_ALERT "[wq] unregistered\n");
}

module_init(wq_init);
module_exit(wq_exit);
