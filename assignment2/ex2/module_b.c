#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tra Vu");
MODULE_DESCRIPTION("Kernel Module B - Uses function from Module A");


extern int add_numbers(int a, int b);

static int __init module_b_init(void) {
    int result = add_numbers(3, 7);
    printk(KERN_INFO "Module B loaded. add_numbers(3,7) = %d\n", result);
    return 0;
}

static void __exit module_b_exit(void) {
    printk(KERN_INFO "Module B unloaded.\n");
}

module_init(module_b_init);
module_exit(module_b_exit);
