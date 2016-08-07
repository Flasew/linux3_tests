#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/workqueue.h>


MODULE_LICENSE("Dual BSD/GPL");

void test(void* args){
	printk(KERN_INFO "hello from a workqueue\n");
}

static DECLARE_WORK(ws, test);
static struct workqueue_struct* wq;

irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	queue_work(wq,&ws);
	return (irq_handler_t) IRQ_HANDLED;
}

static int __init wq_init(void) {

	free_irq(16,NULL);

	request_irq(16,
		(irq_handler_t) isr,
		IRQF_TRIGGER_RISING,
		"wq_test",
		NULL);

	wq = create_workqueue("wq");

	printk(KERN_INFO "wq registered\n");

	return 0;
}

static void __exit wq_exit(void) {
	free_irq(16,NULL);
	flush_workqueue(wq);
	destroy_workqueue(wq);
	printk(KERN_INFO "wq unregistered\n");
}

module_init(wq_init);
module_exit(wq_exit);
