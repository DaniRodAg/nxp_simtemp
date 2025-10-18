#include <linux/kernel.h>		/*Needed for Kern alert*/
#include "nxp_simtemp.h"

//Waitqueue
static DECLARE_WAIT_QUEUE_HEAD(temp_waitqueue);
static DEFINE_SPINLOCK(data_lock);

/* BUFFER FOR DATA STORAGE */
static char kernel_buffer[BUF_COUNT][BUF_SIZE];
static int head = 0;
static int tail = 0;
static int count = 0;

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
static int threshold_mC = 36000;
static bool threshold_alert = 0;

DEVICE_INT_ATTR(threshold_mC, 0660, threshold_mC);

/* HR timer global variables*/
static struct hrtimer my_hrtimer;
ktime_t interval;
static int temp = 0;

struct timespec64 ts;
struct tm tm;

/* HR timer functions*/

DECLARE_WORK(workqueue,my_work_handler);

static void my_work_handler(struct work_struct *work)
{
	unsigned long flags;
	
	/* Store current time and data into circular buffer */
	spin_lock_irqsave(&data_lock, flags);

	/* Print current time and data */
	scnprintf(kernel_buffer[head],BUF_COUNT,
		"%04ld-%02d-%02d %02d:%02d:%02d.%03ld UTC temp=%03d.%dC alert=%d\n",
	        tm.tm_year + 1900,
	        tm.tm_mon + 1,
        	tm.tm_mday,
	        tm.tm_hour,
        	tm.tm_min,
	        tm.tm_sec,
        	ts.tv_nsec / 1000000, temp/1000,temp%1000,
		threshold_alert);
	
	head = (head + 1) % BUF_COUNT;
	if (count < BUF_COUNT)
        	count++;
	else
        	tail = (tail + 1) % BUF_COUNT; // overwrite oldest
	spin_unlock_irqrestore(&data_lock, flags);
	
	pr_info("%s", kernel_buffer[(head - 1 + BUF_COUNT) % BUF_COUNT]);
	
	if (temp > threshold_mC && !threshold_alert) {
		threshold_alert = 1;
		wake_up_interruptible(&temp_waitqueue); // notify waiting processes
		pr_info("Temperature threshold exceeded: %d\n", temp);
	}
}

/* HR timer callback for periodic timer */
static enum hrtimer_restart my_hrtimer_handler(struct hrtimer *timer)
{

	/* Get current epoch time */
	ktime_get_real_ts64(&ts);
	
	/* Epoch to UTC*/
	time64_to_tm(ts.tv_sec, 0, &tm);
	
	/* TEMPERATURE MODE SELECTION */
	if(!strcasecmp(mode_buf, "RAMP"))
		temp += 100;				// Increment temperature in 100mdegC
	else if (!strcasecmp(mode_buf, "NOISY"))
		temp = get_random_u32() % 100000;	// Random temperature values
	else if	(!strcasecmp(mode_buf, "NORMAL")) 
		temp = 25000;				// Set temperature to 25degC
	else 						// If an incorrect mode is selected
	{						//set to normal

		pr_info("Unknown mode: %s, resetting to NORMAL\n", mode_buf);
		strcpy(mode_buf, "NORMAL");
		temp = 25000;
	}
	
	/* Schedule work qeue to avoid wedge*/
	schedule_work(&workqueue);
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
        char kbuf[BUF_COUNT * BUF_SIZE];
        size_t total_len = 0;
        int i;
        unsigned long flags;

        if (*off > 0)
                return 0; // only allow one read per open()

        spin_lock_irqsave(&data_lock, flags);
        for (i = 0; i < count; i++) {
                int idx = (tail + i) % BUF_COUNT;
                total_len += scnprintf(kbuf + total_len, sizeof(kbuf) - total_len, "%s", kernel_buffer[idx]);
                if (total_len >= sizeof(kbuf))
                        break;
        }
        spin_unlock_irqrestore(&data_lock, flags);

        if (copy_to_user(buf, kbuf, total_len))
                return -EFAULT;
	
	// reset event flag
	threshold_alert = 0;
	
        *off += total_len;
        return total_len;
}

/*
** @brief This function is called, when the app calls the poll function
*/
static unsigned int my_poll(struct file *filp, struct poll_table_struct *wait)
{
	__poll_t mask = 0;
	poll_wait(filp, &temp_waitqueue, wait);

	if (threshold_alert)
		mask |= POLLIN | POLLRDNORM; // readable

	return mask;
}



/*
** @brief This function is called, when the module is loaded into the kernel
*/

static int __simtemp_init(void)
{
	/*Init miscdevice */
	pr_info("simtemp - Register misc device\n");

	// Register misc char device
	if(misc_register(&simtemp_device)) {
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
	my_hrtimer.function = &my_hrtimer_handler;
	hrtimer_start(&my_hrtimer, interval, HRTIMER_MODE_REL);

	return 0;
}


/*
** @brief This function is called, when the module is removed from the kernel
*/

static void __simtemp_exit(void)
{
	hrtimer_cancel(&my_hrtimer);

	device_remove_file(simtemp_device.this_device, &dev_attr_sampling_ms.attr);
	device_remove_file(simtemp_device.this_device, &dev_attr_threshold_mC.attr);
	device_remove_file(simtemp_device.this_device, &dev_attr_mode);
	misc_deregister(&simtemp_device);
	pr_notice("simtemp - Goodbye, Kernel!\n");
}

module_init(__simtemp_init);
module_exit(__simtemp_exit);




