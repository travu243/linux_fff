#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>

extern spinlock_t global_spinlock;

static int __init mod_b_init(void)
{
    printk(KERN_INFO "mod_b: trying to acquire spinlock\n");

    spin_lock(&global_spinlock);  // Sẽ bị bận chờ nếu mod_a đang giữ lock
    printk(KERN_INFO "mod_b: spinlock acquired\n");

    spin_unlock(&global_spinlock);
    printk(KERN_INFO "mod_b: spinlock released\n");

    return 0;
}

static void __exit mod_b_exit(void)
{
    printk(KERN_INFO "mod_b: exiting\n");
}

module_init(mod_b_init);
module_exit(mod_b_exit);
MODULE_LICENSE("GPL");
