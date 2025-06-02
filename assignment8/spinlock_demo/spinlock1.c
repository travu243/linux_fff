#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

spinlock_t global_spinlock;

static int __init mod_a_init(void)
{
    printk(KERN_INFO "mod_a: initializing and locking spinlock for 10 seconds\n");

    spin_lock_init(&global_spinlock);

    spin_lock(&global_spinlock);
    printk(KERN_INFO "mod_a: spinlock acquired\n");

    // Giữ lock 10 giây
    msleep(10000);

    spin_unlock(&global_spinlock);
    printk(KERN_INFO "mod_a: spinlock released\n");

    return 0;
}

static void __exit mod_a_exit(void)
{
    printk(KERN_INFO "mod_a: exiting\n");
}

EXPORT_SYMBOL(global_spinlock);

module_init(mod_a_init);
module_exit(mod_a_exit);
MODULE_LICENSE("GPL");
