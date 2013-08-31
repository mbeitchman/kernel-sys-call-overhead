
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */
#include <asm/uaccess.h>	/* for copy_from_user */

#define procfs_name "delay_mod"
#define PROCFS_MAX_SIZE		1024

struct proc_dir_entry *Our_Proc_File;
static char procfs_buffer[PROCFS_MAX_SIZE];
long delay_knob_val = 0;
static unsigned long procfs_buffer_size = 0;
long globalval = 0;

void(*old_fn_ptr)(void) = NULL;
extern void(*marc_fn_ptr)(void);

void internal_delay(void)
{
	long i = 0;
	long j = 0;
	long localval = 0;
	
	//printk(KERN_INFO "delay_knob = %ld", delay_knob_val);

	for(i = 0; i < delay_knob_val; i++)
	{
		for(j = 0; j < 1000; j++)
		{
			globalval *= i*j;
		}
	}
	
	//printk(KERN_INFO "local_val = %ld", localval);

	//globalval = localval;
}

int
procfile_read(char *buffer,
	      char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;
	
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);

	if (offset > 0) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		ret = sprintf(buffer, "%ld\n", delay_knob_val);
	}

	return ret;
}

int procfile_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	/* get buffer size */
	procfs_buffer_size = count;
	if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
		procfs_buffer_size = PROCFS_MAX_SIZE;
	}
	
	/* write data to the buffer */
	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
		return -EFAULT;
	}
	
	delay_knob_val = simple_strtol(buffer, NULL, 10);

	return procfs_buffer_size;
}


int init_module(void)
{
	Our_Proc_File = create_proc_entry(procfs_name, 0644, NULL);
	
	if (Our_Proc_File == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
		       procfs_name);
		return -ENOMEM;
	}

	old_fn_ptr = marc_fn_ptr;
	marc_fn_ptr = &internal_delay;

	Our_Proc_File->read_proc = procfile_read;
	Our_Proc_File->write_proc = procfile_write;
	//Our_Proc_File->owner 	 = THIS_MODULE;
	Our_Proc_File->mode 	 = S_IFREG | S_IRUGO | S_IWUGO;
	Our_Proc_File->uid 	 = 0;
	Our_Proc_File->gid 	 = 0;
	Our_Proc_File->size 	 = 37;

	printk(KERN_INFO "/proc/%s created\n", procfs_name);	
	return 0;	/* everything is ok */
}

void cleanup_module(void)
{
	marc_fn_ptr = old_fn_ptr;
	remove_proc_entry(procfs_name, NULL);
	printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}


