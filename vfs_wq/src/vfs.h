#ifndef __VFS__
#define __VFS__

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/fcntl.h>

struct file* file_open(const char* path, int flags, int rights);
void file_close(struct file* file);
int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
int file_sync(struct file* file);

#endif
