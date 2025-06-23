/*
 *  hello_world.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

static int __init Init_module(void)
{
	printk(KERN_INFO "Hello world\n");

	/*
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

static void __exit Cleanup_module(void)
{
	printk(KERN_INFO "Fuck world\n");
}

module_init(Init_module);
module_exit(Cleanup_module);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRA VU");
MODULE_DESCRIPTION("LED OPZ3");
