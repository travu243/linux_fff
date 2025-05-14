#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tra Vu");
MODULE_DESCRIPTION("Kernel Module A - Exports a function");



int add_numbers(int a, int b) {
    return a + b;
}

EXPORT_SYMBOL(add_numbers);

static int __init module_a_init(void) {
    printk(KERN_INFO "Module A loaded.\n");
    return 0;
}

static void __exit module_a_exit(void) {
    printk(KERN_INFO "Module A unloaded.\n");
}

module_init(module_a_init);
module_exit(module_a_exit);
