#include <linux/io.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#ifndef __KERNEL__
    #define __KERNEL__
#endif //!__KERNEL__

#ifndef MODULE
    #define MODULE
#endif //!MODULE
static int gnOffset = 0;
static int gnOffsetRead = 0;
 
MODULE_AUTHOR ("Jammy");
MODULE_LICENSE("GPL");
module_param (gnOffset, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (gnOffset, "current buffer write offset");
module_param (gnOffsetRead, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC (gnOffsetRead, "current buffer read offset");

#define MAX_PROC_SIZE 100
static char proc_data[MAX_PROC_SIZE];

 

int read_proc(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int len=gnOffset - gnOffsetRead;
	if(len <=0){
		//next time from beginning
		gnOffsetRead = 0; 
		return 0;
	}
	if (len > count) len=count;
	if(copy_to_user(buf, proc_data+gnOffsetRead, len)) {
		return -EFAULT;
	}
	gnOffsetRead += len; 
	return len;
}

//when write proce, host will keep calling until return -1
int write_proc(struct file *file, const char __user *buf,
                           size_t count, loff_t *ppos)
 
{

	if(count + gnOffset > MAX_PROC_SIZE){
		//next time start from top
  		printk("buffer full. current level is %d\n", gnOffset);
		return -EFAULT;
 
	}
 
	if(copy_from_user(proc_data+gnOffset, buf, count))
		return -EFAULT;

	gnOffset+= count;
	return count;
}
static int hello_proc_show(struct seq_file *m, void *v) {
  printk("Hello proc!\n");
  return 0;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, hello_proc_show, NULL);
}
static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = hello_proc_open,
  .read = read_proc,
  .write = write_proc,
  .llseek = seq_lseek,
  .release= single_release,
 
};

 
int Initial_Module(void)
{

	printk("Initial_Module %d \n", gnOffset);
	proc_create("hello",0666,NULL, &proc_fops);
	
	printk("proc initialized");
	return 0;
}

void Cleanup_Module(void)
{
	printk("Cleanup_Module\n");
	remove_proc_entry("hello",NULL);
}

module_init(Initial_Module);
module_exit(Cleanup_Module);

