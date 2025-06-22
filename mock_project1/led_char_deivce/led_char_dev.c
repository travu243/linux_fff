#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DEVICE_NAME "ledchar"
#define CLASS_NAME  "led"
#define BUF_SIZE    2  // 0/1

#define GPIO0_BASE    0x44E07000
#define GPIO_OE       0x134
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT   0x194
#define GPIO0_30      (1 << 30)

static dev_t dev_num;
static struct class *led_class;
static struct cdev led_cdev;
static void __iomem *gpio_base;

static int led_open(struct inode *inode, struct file *file) {
    pr_info("ledchar: open\n");
    return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char kbuf[BUF_SIZE] = {};
    if (count < 1 || count > BUF_SIZE) return -EINVAL;
    if (copy_from_user(kbuf, buf, count)) return -EFAULT;

    if (kbuf[0] == '1') {
        iowrite32(GPIO0_30, gpio_base + GPIO_SETDATAOUT);
        pr_info("ledchar: on\n");
    } else if (kbuf[0] == '0') {
        iowrite32(GPIO0_30, gpio_base + GPIO_CLEARDATAOUT);
        pr_info("ledchar: off\n");
    } else return -EINVAL;

    return count;
}

static int led_release(struct inode *inode, struct file *file) {
    pr_info("ledchar: close\n");
    return 0;
}

static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = led_open,
    .write   = led_write,
    .release = led_release
};

static int __init led_init(void) {
    u32 v;

    // sign up char device
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)) {
        pr_err("ledchar: failed alloc dev region\n");
        return -1;
    }
    cdev_init(&led_cdev, &fops);
    if (cdev_add(&led_cdev, dev_num, 1)) {
        pr_err("ledchar: cdev_add failed\n");
        unregister_chrdev_region(dev_num,1);
        return -1;
    }

    led_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_class)) {
        pr_err("ledchar: class create failed\n");
        cdev_del(&led_cdev);
        unregister_chrdev_region(dev_num,1);
        return PTR_ERR(led_class);
    }
    device_create(led_class, NULL, dev_num, NULL, DEVICE_NAME);

    // reference register GPIO
    gpio_base = ioremap(GPIO0_BASE, 0x1000);
    if (!gpio_base) {
        pr_err("ledchar: ioremap failed\n");
        device_destroy(led_class, dev_num);
        class_destroy(led_class);
        cdev_del(&led_cdev);
        unregister_chrdev_region(dev_num,1);
        return -ENOMEM;
    }

    v = ioread32(gpio_base + GPIO_OE);
    v &= ~GPIO0_30;
    iowrite32(v, gpio_base + GPIO_OE);
    pr_info("ledchar: initialized, /dev/%s ready\n", DEVICE_NAME);
    return 0;
}

static void __exit led_exit(void) {
    // turn off
    iowrite32(GPIO0_30, gpio_base + GPIO_CLEARDATAOUT);
    iounmap(gpio_base);

    device_destroy(led_class, dev_num);
    class_destroy(led_class);
    cdev_del(&led_cdev);
    unregister_chrdev_region(dev_num,1);
    pr_info("ledchar: exited\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dangcap");
MODULE_DESCRIPTION("LED GPIO Char Device");
