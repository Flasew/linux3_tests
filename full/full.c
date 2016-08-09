#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <linux/fcntl.h>
#include <linux/ktime.h>

#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#define IRQNUMBER 40

MODULE_LICENSE("Dual BSD/GPL");

static struct workqueue_struct* wq;
static struct file* f;
static int i;

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_sync(struct file* file) {
    vfs_fsync(file, 0);
    return 0;
}

void test(void* args){

	static ktime_t t0;
	static ktime_t t1;

	t0 = ktime_get();

	file_write(f, 16*i, (unsigned char *) &t0, 8);

	t1 = ktime_get();	

	file_write(f, (16*i)+8, (unsigned char *) &t1, 8);

	file_sync(f);
	i += 1;
}

static DECLARE_WORK(ws, test);

irq_handler_t isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	queue_work(wq,&ws);
	return (irq_handler_t) IRQ_HANDLED;
}

static int __init full_init(void) {

	int ok;

	i = 0;

	ok = request_irq(IRQNUMBER,
		(irq_handler_t) isr,
		0x2080, /* IRQF_SHARED | IRQF_ONESHOT */
		"full_test",
		IRQF_NO_SUSPEND);

	wq = create_workqueue("wq");

	f = file_open("full_test.out", O_WRONLY | O_CREAT, S_IRWXO);

	printk(KERN_ALERT "[full] request_irq: %d | create_workqueue: %d", ok,wq);

	printk(KERN_ALERT "[full] registered\n");

	return 0;
}

static void __exit full_exit(void) {
	flush_workqueue(wq);
	destroy_workqueue(wq);
	file_close(f);
	printk(KERN_ALERT "[full] unregistered\n");
}

module_init(full_init);
module_exit(full_exit);
