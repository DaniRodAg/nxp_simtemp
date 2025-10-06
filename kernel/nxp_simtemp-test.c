#include <linux/module.h>		/*To include a header file located under include\linux\ */
#include <linux/kernel.h>		/*Needed for Kern alert*/
#include <linux/init.h>			//
#include <linux/types.h>		//
#include <linux/fs.h>			//
#include <linux/string.h>

#include <linux/hrtimer.h>

#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>

#include <linux/wait.h>                 //Required for the wait queues
#include <linux/poll.h>

#include <linux/sysfs.h> 
#include <linux/kobject.h>

#include <linux/err.h>


#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>


/* METADATA */
MODULE_LICENSE("GPL");			/*Open source license*/
MODULE_AUTHOR("Daniel Rodriguez");
MODULE_DESCRIPTION("Virtual Sensor + Alert Path");

/* global variable for sysfs folder simtemp */
static struct kobject *dummy_kobj;

/**
 * @brief Read callback
 */
static ssize_t dummy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer){
	return sprintf(buffer, "You have read from /sys/kernel/%s%s\n", kobj->name, attr->attr.name);
}


/**
 * @brief Write callback
 */
static ssize_t dummy_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count){
	pr_info("simtemp - You wrote '%s' to /sys/kernel/%s%s\n", buffer, kobj->name, attr->attr.name);
	return count;
}


static struct kobj_attribute dummy_attr = __ATTR(dummy, 0660, dummy_show, dummy_store);
/*
static struct of_device_id mxs_auart_dt_ids[] = { 
	{
 		.compatible = "fsl,imx28-auart", 
	}, {/* sentinel } 
}; 
MODULE_DEVICE_TABLE(of, mxs_auart_dt_ids); 
[...] 
	static struct platform_driver mxs_auart_driver = { 
		.probe = mxs_auart_probe, 
		.remove = mxs_auart_remove, 
		.driver = 
		{
			.name = "mxs-auart", 
			.of_match_table = mxs_auart_dt_ids, 
		}, 
};
*/


/* character device information*/
static __u32 major_dev_nu;

/* Read/Write character device */
static char timer_buff[64];

/* HR timer global variables*/
static struct hrtimer my_hrtimer;
static u64 start_t;
static u64 hrtimer_counter = 0;
static u64 temp_buffe[20];

/* HR timer */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer) {
	/* Get current time */
	u64 now_t = jiffies;
	pr_info("simtemp - %llu:now_t = %u \n", hrtimer_counter, jiffies_to_msecs(now_t - start_t));
	if (hrtimer_counter) {
		hrtimer_counter--;
		return HRTIMER_RESTART;
	}
	else {
		return HRTIMER_NORESTART;
	}
	

}


static ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *offset)
{
	int not_copied, delta, to_copy = (len + *offset) < sizeof(timer_buff) ? len : (sizeof(timer_buff) - *offset);
	
	pr_info("simtemp - Read is called. Read: %ld bytes. copying %d bytes. Offset: %lld\n", len, to_copy, *offset);
	
	if (*offset >= sizeof(timer_buff))
		return 0;
	not_copied = copy_to_user(user_buf, &timer_buff[*offset], to_copy);
	delta = to_copy - not_copied;
	if(not_copied)
		pr_warn("simtemp - could only copy %d bytes\n",delta);

	*offset += delta;

	return delta;
}


static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *offset)
{
	int not_copied, delta, to_copy = (len + *offset) < sizeof(timer_buff) ? len : (sizeof(timer_buff) - *offset);
	
	pr_info("simtemp - Write is called. Wrote: %ld bytes. copying %d bytes. Offset: %lld\n", len, to_copy, *offset);
	
	if (*offset >= sizeof(timer_buff))
		return 0;
	not_copied = copy_from_user(&timer_buff[*offset], user_buf, to_copy);
	delta = to_copy - not_copied;
	if(not_copied)
		pr_warn("simtemp - could only copy %d bytes\n",delta);
	
	*offset += delta;
	
	return delta;
}
/*
static int my_open(struct inode *inode, struct file *filp)
{
	pr_info("simtemp - Major: %d, Minor: %d\n", imajor(inode), iminor(inode));

	pr_info("simtemp - flip->f_pos: %lld\n", filp->f_pos);
	pr_info("simtemp - flip->f_mode: 0x%x\n", filp->f_mode);
	pr_info("simtemp - flip->f_flags: 0x%x\n", filp->f_mode);

	return 0;
}


static int my_release(struct inode *inode, struct file *filp)
{
	pr_info("simtemp - File is closed\n");
	return 0;
}
*/

static const struct file_operations fops = {
	.read = my_read,
	.write = my_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */

static int __simtemp_init(void)
{
	/* Init of hrtimer */
	hrtimer_counter = 10;
	hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	my_hrtimer.function = &test_hrtimer_handler;
	start_t = jiffies;
	hrtimer_start(&my_hrtimer, ms_to_ktime(200), HRTIMER_MODE_REL);
	
	/*creating folder simtemp*/
	dummy_kobj = kobject_create_and_add("simtemp",kernel_kobj);
	if(!dummy_kobj) {
		pr_info("simtemp - Error creating /sys/kernel/simtemp\n");
		return ENOMEM;
	}
	
	/* Create the sysfs file dummy */
	if(sysfs_create_file(dummy_kobj, &dummy_attr.attr)){
		pr_info("simtemp - Error creating /sys/kernel/simtemp/dummy\n");
		kobject_put(dummy_kobj);
		return ENOMEM;
	}


	/* Init device char*/
	major_dev_nu = register_chrdev(0, "simtemp", &fops);
	if (major_dev_nu < 0) {
		pr_err("simtemp - Error registering chrdev\n");
		return major_dev_nu;
	}
	pr_info("simtemp - Major Device Number: %d\n", major_dev_nu);
	
	return 0;
}


/**
 * @brief This function is called, when the module is removed from the kernel
 */

static void __simtemp_exit(void)
{
	hrtimer_cancel(&my_hrtimer);
	sysfs_remove_file(dummy_kobj, &dummy_attr.attr);
	kobject_put(dummy_kobj);
	unregister_chrdev(major_dev_nu, "simtemp");
	pr_notice("simtemp - Goodbye, Kernel!\n");
}

module_init(__simtemp_init);
module_exit(__simtemp_exit);




