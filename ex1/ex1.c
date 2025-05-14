/*
 *  hello_world.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

static int __init Init_module(void)
{
	printk(KERN_INFO "Hello world info\n");
	printk(KERN_WARNING "Hello world warning\n");
	printk(KERN_ERR "Hello world error\n");
	printk(KERN_DEBUG "Hello world debug\n");

	/*
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

static void __exit Cleanup_module(void)
{
	printk(KERN_INFO "bye world info\n");
	printk(KERN_WARNING "Goodbye world warning\n");
        printk(KERN_ERR "Goodbye world error\n");
        printk(KERN_DEBUG "Goodbye world debug\n");
}

module_init(Init_module);
module_exit(Cleanup_module);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRA VU");
MODULE_DESCRIPTION("LOG_LEVEL");
