#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <linux/fcntl.h>


MODULE_LICENSE("Dual BSD/GPL");



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

void file_test(void){

	struct file* f;
	unsigned int size;
	unsigned long long offset;
	unsigned char* data;
	char * fname;

	int flags;
	int mode;

	fname = "vfs_test.out";
	data =  "Hello, world!";

	flags = O_WRONLY | O_CREAT;
	mode = S_IRWXO;

	offset = 0;
	size = 13;

	f = file_open(fname, flags, mode);

	file_write(f, offset, data, size);

	file_close(f);

}

static int __init vfs_init(void) {
	printk(KERN_INFO "Hello world! registered\n");
	file_test();
	return 0;
}

static void __exit vfs_exit(void) {
	printk(KERN_INFO "Hello world! unregistered\n");
}

module_init(vfs_init);
module_exit(vfs_exit);