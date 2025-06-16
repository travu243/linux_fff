#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>//copy_to_user,copy_from_user
#include <linux/device.h>

#define BUF_SIZE 1024

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char device_buffer[BUF_SIZE];
static size_t buffer_size = 0;

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "device closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    ssize_t ret;

    if (*ppos >= buffer_size)
        return 0;

    if (count > buffer_size - *ppos)
        count = buffer_size - *ppos;

    if (copy_to_user(buf, device_buffer + *ppos, count))
        return -EFAULT;

    *ppos += count;
    ret = count;

    printk(KERN_INFO "read %zu bytes\n", ret);
    return ret;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    //ssize_t ret;

    if (count > BUF_SIZE)
        count = BUF_SIZE;

    if (copy_from_user(device_buffer, buf, count))
        return -EFAULT;

    buffer_size = count;
    *ppos = 0;

    printk(KERN_INFO "wrote %zu bytes\n", count);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init my_init(void) {
    int result;

    result = alloc_chrdev_region(&dev_num, 0, 1, "my_char_device");
    if (result < 0) {
        printk(KERN_ALERT "failed to allocate major number\n");
        return result;
    }

    cdev_init(&my_cdev, &fops);
    result = cdev_add(&my_cdev, dev_num, 1);
    if (result < 0) {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "failed to add cdev\n");
        return result;
    }

    my_class = class_create("my_class");
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "failed to create class\n");
        return PTR_ERR(my_class);
    }

    my_device = device_create(my_class, NULL, dev_num, NULL, "my_char_device");
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "failed to create device\n");
        return PTR_ERR(my_device);
    }

    printk(KERN_INFO "module loaded. major: %d minor: %d\n", MAJOR(dev_num), MINOR(dev_num));
    return 0;
}

static void __exit my_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("ex2");
