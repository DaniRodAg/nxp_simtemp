/* SPDX-License-Identifier: GPL-2.0 */
/*
 * nxp_simtemp.h - Example header for NXP temperature driver
 */

#ifndef _NXP_SIMTEMP_H
#define _NXP_SIMTEMP_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>

#include <linux/string.h>

#include <linux/fs.h>
#include <linux/slab.h>         /* kmalloc(), kfree() */
#include <linux/uaccess.h>      /* copy_to_user(), copy_from_user() */

#include <linux/hrtimer.h>
#include <linux/kthread.h>
#include <linux/wait.h>         /* wait queues */
#include <linux/poll.h>
#include <linux/timekeeping.h>

#include <linux/miscdevice.h>

#include <linux/err.h>
#include <linux/random.h>

/* METADATA */
MODULE_LICENSE("GPL");                  /*Open source license*/
MODULE_AUTHOR("Daniel Rodriguez");
MODULE_DESCRIPTION("Virtual Sensor + Alert Path");

/* --- Defines ---*/
#define TXT_BUF_SIZE 10

/* --- Function prototypes --- */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static unsigned int my_poll(struct file *filp, struct poll_table_struct *wait);



//static int my_open(struct inode *inode, struct file *file);
//static int my_close(struct inode *inode, struct file *file);
//static ssize_t my_write(struct file *filp,
//                        const char __user *user_buffer,
//                        size_t size, loff_t *ppos);
//static ssize_t my_read(struct file *file,
//                       char __user *user_buffer,
//                       size_t size, loff_t *file_pos);


#endif /* _NXP_SIMTEMP_H */
