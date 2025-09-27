#include <linux/module.h>	//To include a header file located under include/linux/
#include <linux/init.h>		//
#include <linux/types.h>

static int __simtemp_init(void)
{
	printk("Hello, kernel!\n");
	return 0;
}

static void __simtemp_exit(void)
{
	printk("Goodbye, Kernel!\n");
}

module_init(__simtemp_init);
module_exit(__simtemp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Rodriguez");
MODULE_DESCRIPTION("Virtual Sensor + Alert Path");


