#include <linux/module.h>          // module_init, module_exit, MODULE_*
#include <linux/kernel.h>          // pr_info, KERN_INFO (log)
//#include <linux/init.h>            // __init, __exit
#include <linux/fs.h>              // alloc_chrdev_region, struct file_operations
#include <linux/cdev.h>            // struct cdev, cdev_init, cdev_add, cdev_del
#include <linux/device.h>          // class_create, device_create, device_destroy
#include <linux/uaccess.h>         // copy_from_user, copy_to_user
#include <linux/platform_device.h> // platform_driver, platform_device, struct resource
#include <linux/io.h>              // ioremap, iowrite32, ioread32, iounmap
#include <linux/of.h>              // device tree: of_match_ptr, of_device_id
#include <linux/of_address.h>      // of_iomap

#define DEVICE_NAME "ledled"
#define CLASS_NAME "ledclass"

#define LED_IOCTL_SET _IOW(99, 0, int)
#define LED_IOCTL_GET _IOR(99, 1, int)

static dev_t dev_num;
static struct cdev led_cdev;
static struct class *led_class;

static struct {
    void __iomem *base;
    u32 set, clr, oe;
    u32 bit;
    bool state;
} led;



void set_led_state(bool a){
    if (a) iowrite32((1 << led.bit), led.base + led.set);
    else iowrite32((1 << led.bit), led.base + led.clr);

    led.state = a;
}
EXPORT_SYMBOL(set_led_state);



//ioctl
static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int val;

    switch (cmd) {
    case LED_IOCTL_SET:
        if (copy_from_user(&val, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (val)
            iowrite32((1 << led.bit), led.base + led.set);
        else
            iowrite32((1 << led.bit), led.base + led.clr);
        led.state = !!val;
        break;

    case LED_IOCTL_GET:
        val = led.state;
        if (copy_to_user((int __user *)arg, &val, sizeof(int)))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

//char device read/write
static ssize_t led_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    char c = led.state ? '1' : '0';
    if (*off > 0) return 0;
    *off = 1;
    return copy_to_user(buf, &c, 1) ? -EFAULT : 1;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    char c;
    if (copy_from_user(&c, buf, 1)) return -EFAULT;

    if (c == '1') {
        iowrite32((1 << led.bit), led.base + led.set);
        led.state = true;
    } else if (c == '0') {
        iowrite32((1 << led.bit), led.base + led.clr);
        led.state = false;
    }
    return len;
}

static int led_open(struct inode *inode, struct file *file) { return 0; }
static int led_release(struct inode *inode, struct file *file) { return 0; }

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .read = led_read,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
};

static int led_probe(struct platform_device *pdev){
    struct device *dev = &pdev->dev;

    led.base = of_iomap(dev->of_node, 0);
    if (!led.base) return -ENOMEM;

    of_property_read_u32(dev->of_node, "bit", &led.bit);
    of_property_read_u32(dev->of_node, "set-offset", &led.set);
    of_property_read_u32(dev->of_node, "clr-offset", &led.clr);
    of_property_read_u32(dev->of_node, "oe-offset", &led.oe);

    // set output
    u32 v = ioread32(led.base + led.oe);
    v &= ~(1 << led.bit);
    iowrite32(v, led.base + led.oe);

    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&led_cdev, &fops);
    cdev_add(&led_cdev, dev_num, 1);

    led_class = class_create(CLASS_NAME);
    device_create(led_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("ledled device created\n");
    return 0;
}

static void led_remove(struct platform_device *pdev){
    device_destroy(led_class, dev_num);
    class_destroy(led_class);
    cdev_del(&led_cdev);
    unregister_chrdev_region(dev_num, 1);

    if (led.base)
        iounmap(led.base);

    return;
}

static const struct of_device_id led_of_match[] = {
    { .compatible = "tra,gpio-led" },
    { }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .driver = {
        .name = "ledled_driver",
        .of_match_table = led_of_match,
    }
};

module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("LEDLEDLED");
