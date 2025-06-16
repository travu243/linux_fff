#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define IOCTL_NUMBER "my_device"
#define BUF_SIZE 1024

#define MAGIC_NUM 0xF5
#define IOCTL_CLEAR_BUFFER _IO(IOCTL_NUMBER, 0)
#define IOCTL_GET_SIZE     _IOR(IOCTL_NUMBER, 1, int)

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

static char device_buffer[BUF_SIZE];
static size_t buffer_size = 0;

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "device closed\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    if (len > BUF_SIZE) len = BUF_SIZE;
    if (copy_from_user(device_buffer, buf, len))
        return -EFAULT;

    buffer_size = len;
    printk(KERN_INFO "wrote %zu bytes\n", len);
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    if (*offset >= buffer_size)
        return 0; //  EOF

    if (len > buffer_size - *offset)
        len = buffer_size - *offset;

    if (copy_to_user(buf, device_buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    printk(KERN_INFO "read %zu bytes\n", len);
    return len;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case IOCTL_CLEAR_BUFFER:
            memset(device_buffer, 0, BUF_SIZE);
            buffer_size = 0;
            printk(KERN_INFO "buffer cleared\n");
            break;

        case IOCTL_GET_SIZE:
            if (copy_to_user((int __user *)arg, &buffer_size, sizeof(int)))
                return -EFAULT;
            printk(KERN_INFO "returned buffer size = %zu\n", buffer_size);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .unlocked_ioctl = my_ioctl,
};

static int __init my_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) return ret;

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    if (cdev_add(&my_cdev, dev_num, 1) < 0)
        goto r_unreg;

    my_class = class_create("my_class");
    if (IS_ERR(my_class))
        goto r_cdev;

    device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    printk(KERN_INFO "module loaded\n");
    return 0;

r_cdev:
    cdev_del(&my_cdev);
r_unreg:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("ex3 asm11");
