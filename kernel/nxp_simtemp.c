#include <linux/kernel.h>		/*Needed for Kern alert*/
#include "nxp_simtemp.h"

//Waitqueue
DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx_data);

/* BUFFER FOR DATA STORAGE */
static char *data_buffer;
static char kernel_buffer[64];
static DEFINE_SPINLOCK(data_lock);

/* DEVICE CONTROLS */
static char mode_buf[TXT_BUF_SIZE] = "RAMP";
static bool can_write = false;
static bool can_read  = false;


/* MODE ATTRIBUTES */
ssize_t	mode_store(struct device *d, struct device_attribute *a, 
		   const char *buf, size_t len)
{
	int to_copy = len < sizeof(mode_buf) ? len : sizeof(mode_buf);
	strncpy(mode_buf, buf, to_copy);
	mode_buf[strcspn(mode_buf, "\n")] = '\0';
	return to_copy;
}

ssize_t mode_show(struct device *d, struct device_attribute *a, 
                   char *buf)
{
	strcpy(buf, mode_buf);
	return strlen(mode_buf);
}

DEVICE_ATTR(mode, 0660, mode_show, mode_store);

/* SAMPLING ATTRIBUTES */
static int sampling_ms = 2000;
DEVICE_INT_ATTR(sampling_ms, 0660, sampling_ms);

/* THRESHOLD ALERT ATTRIBUTES */
static int threshold_mC = 0;
DEVICE_INT_ATTR(threshold_mC, 0660, threshold_mC);

/* HR timer global variables*/
static struct hrtimer my_hrtimer;
ktime_t interval;
static int temp = 0;

struct timespec64 ts;
struct tm tm;

/* HR timer functions*/

/* HR timer callback for periodic timer */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer)
{
	/* Get current epoch time */
	ktime_get_real_ts64(&ts);
	/* Epoch to UTC*/
	time64_to_tm(ts.tv_sec, 0, &tm);
	
	/* TEMPERATURE MODE SELECTION */
	if(!strcasecmp(mode_buf, "RAMP"))
		temp += 100;				// Increment temperature in 100mdegC
	else if (!strcasecmp(mode_buf, "NOISY"))
		temp = get_random_u32();		// Random temperature values
	else if	(!strcasecmp(mode_buf, "NORMAL")) 
		temp = 25000;				// Set temperature to 25degC
	else 						// If an incorrect mode is selected
	{						//set to normal
		pr_info("%s", mode_buf);
		strcpy(mode_buf, "NORMAL");
		temp = 25000;
	}
	
	/* Print current time and data */
	scnprintf(kernel_buffer,sizeof(kernel_buffer),"%04ld-%02d-%02d %02d:%02d:%02d.%03ld UTC temp = %03d.%dC\n",
	        tm.tm_year + 1900,
	        tm.tm_mon + 1,
        	tm.tm_mday,
	        tm.tm_hour,
        	tm.tm_min,
	        tm.tm_sec,
        	ts.tv_nsec / 1000000, temp/1000,temp%1000);
	pr_info("%s",kernel_buffer);

	/* Re-arm the timer for periodic execution */
	interval = ms_to_ktime(sampling_ms);		// set time interval w input sampl time
	hrtimer_forward_now(timer, interval);		// advance timer to next period

	return HRTIMER_RESTART; 			// keep the timer running
}


/* MISC DEVICE */

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.open = my_open,
	.release = my_release,
	.poll = my_poll
};

static struct miscdevice simtemp_device = {
	.name = "simtemp",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &fops,
};

/*
** @brief This function is called, when the Device file is opened
*/

static int my_open(struct inode *inode, struct file *file)
{
	pr_info("Device File Opened...!!!\n");
	return 0;
}

/*
** @brief This function is called, when the Device file is closed
*/
static int my_release(struct inode *inode, struct file *file)
{
	pr_info("Device File Closed...!!!\n");
	return 0;
}

/*
** @brief This function is called, when the the Device file is read
*/
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{

	int not_copied, delta, to_copy = (len + *off) < sizeof(kernel_buffer) ? len : (sizeof(kernel_buffer) - *off);
	
	pr_info("Read Function\n");
	
	if (*off >= sizeof(kernel_buffer))
                return 0;
        not_copied = copy_to_user(buf, &kernel_buffer[*off], to_copy);
	
	delta = to_copy - not_copied;
        if(not_copied)
                pr_warn("simtemp - could only copy %d bytes\n",delta);

	*off += delta;

	return delta;
}

/*
** @brief This function is called, when the app calls the poll function
*/
static unsigned int my_poll(struct file *filp, struct poll_table_struct *wait)
{
	__poll_t mask = 0;

	poll_wait(filp, &wait_queue_etx_data, wait);
	pr_info("Poll function\n");

	if( can_read )
	{
		can_read = false;
		mask |= ( POLLIN | POLLRDNORM );
	}

	if( can_write )
	{
		can_write = false;
		mask |= ( POLLOUT | POLLWRNORM );
	}
	return mask;
}



/*
** @brief This function is called, when the module is loaded into the kernel
*/

static int __simtemp_init(void)
{
	/*Init miscdevice */
	pr_info("simtemp - Register misc device\n");

	// Allocate a page sized buffer
	data_buffer = (char*) kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!data_buffer)
		return -ENOMEM;

	// Register misc char device
	if(misc_register(&simtemp_device)) {
		kfree(data_buffer);
		return -ENODEV;
	}
	// CREATE SAMPLING TIME FILE 
	if(device_create_file(simtemp_device.this_device, &dev_attr_sampling_ms.attr)) {
                return -ENODEV;
        }
	// CREATE THRESHOLD FILE
	if(device_create_file(simtemp_device.this_device, &dev_attr_threshold_mC.attr)) {
                return -ENODEV;
        }
	// CREATE MODE FILE
	if(device_create_file(simtemp_device.this_device, &dev_attr_mode)) {
                return -ENODEV;
        }



	/* Init of hrtimer */
	ktime_t interval = ms_to_ktime(1000); 
	hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	my_hrtimer.function = &test_hrtimer_handler;
	hrtimer_start(&my_hrtimer, interval, HRTIMER_MODE_REL);

	return 0;
}


/*
** @brief This function is called, when the module is removed from the kernel
*/

static void __simtemp_exit(void)
{
	hrtimer_cancel(&my_hrtimer);
	
	// Free buffer
	kfree(data_buffer);

	device_remove_file(simtemp_device.this_device, &dev_attr_sampling_ms.attr);
	device_remove_file(simtemp_device.this_device, &dev_attr_threshold_mC.attr);
	device_remove_file(simtemp_device.this_device, &dev_attr_mode);
	misc_deregister(&simtemp_device);
	pr_notice("simtemp - Goodbye, Kernel!\n");
}

module_init(__simtemp_init);
module_exit(__simtemp_exit);




