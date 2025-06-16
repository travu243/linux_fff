#include <linux/module.h>       // Core header for modules
#include <linux/fs.h>           // file operations
#include <linux/cdev.h>         // character device structures
#include <linux/uaccess.h>      // copy_to_user/copy_from_user
#include <linux/device.h>       // device creation

#define NUM_DEVICES 4
#define BUF_SIZE 1024

typedef struct {
    dev_t dev_num;
    struct cdev cdev;                      // single cdev structure to handle all devices
    struct class *class;
    struct device *devices[NUM_DEVICES];   // array of device pointers for each minor
    char buffer[NUM_DEVICES][BUF_SIZE];    // data buffers for each minor device
    size_t buf_size[NUM_DEVICES];          // current size of valid data in each buffer
} my_dev_t;

static my_dev_t mydev;  // global instance of our device structure

static int my_open(struct inode *inode, struct file *file) {
    int minor = MINOR(inode->i_rdev);         // get minor number from inode
    file->private_data = (void *)(long)minor; // store minor in file->private_data for later use
    printk(KERN_INFO "device %d opened\n", minor);
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    int minor = (int)(long)file->private_data; // retrieve minor number
    printk(KERN_INFO "device %d closed\n", minor);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int minor = (int)(long)file->private_data;

    // return 0 if reading beyond current data
    if (*ppos >= mydev.buf_size[minor])
        return 0;

    // adjust count if it would exceed buffer
    if (count > mydev.buf_size[minor] - *ppos)
        count = mydev.buf_size[minor] - *ppos;

    // copy data to user-space buffer
    if (copy_to_user(buf, mydev.buffer[minor] + *ppos, count))
        return -EFAULT;

    *ppos += count;  // update read offset
    return count;    // return number of bytes read
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    int minor = (int)(long)file->private_data;

    // limit the number of bytes to write
    if (count > BUF_SIZE)
        count = BUF_SIZE;

    // copy data from user-space buffer
    if (copy_from_user(mydev.buffer[minor], buf, count))
        return -EFAULT;

    mydev.buf_size[minor] = count; // update buffer size
    *ppos = 0;                     // reset offset to 0
    return count;                  // return number of bytes written
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init my_init(void) {
    int ret, i;

    // allocate a range of device numbers
    ret = alloc_chrdev_region(&mydev.dev_num, 0, NUM_DEVICES, "my_multi_dev");
    if (ret < 0) return ret;

    // create a device class for /sys/class
    mydev.class = class_create("my_class");
    if (IS_ERR(mydev.class)) {
        unregister_chrdev_region(mydev.dev_num, NUM_DEVICES);
        return PTR_ERR(mydev.class);
    }

    // initialize the cdev structure with our file operations
    cdev_init(&mydev.cdev, &fops);
    ret = cdev_add(&mydev.cdev, mydev.dev_num, NUM_DEVICES);  // register the cdev for all minors
    if (ret < 0) {
        class_destroy(mydev.class);
        unregister_chrdev_region(mydev.dev_num, NUM_DEVICES);
        return ret;
    }

    // create individual device files for each minor number
    for (i = 0; i < NUM_DEVICES; i++) {
        mydev.devices[i] = device_create(
            mydev.class, NULL, mydev.dev_num + i,
            NULL, "my_char_device%d", i);

        // handle failure and cleanup
        if (IS_ERR(mydev.devices[i])) {
            while (--i >= 0)
                device_destroy(mydev.class, mydev.dev_num + i);
            class_destroy(mydev.class);
            cdev_del(&mydev.cdev);
            unregister_chrdev_region(mydev.dev_num, NUM_DEVICES);
            return PTR_ERR(mydev.devices[i]);
        }
    }

    printk(KERN_INFO "module loaded: %d devices created\n", NUM_DEVICES);
    return 0;
}

static void __exit my_exit(void) {
    int i;

    for (i = 0; i < NUM_DEVICES; i++)
        device_destroy(mydev.class, mydev.dev_num + i);

    class_destroy(mydev.class);
    cdev_del(&mydev.cdev);
    unregister_chrdev_region(mydev.dev_num, NUM_DEVICES);

    printk(KERN_INFO "module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("multiple devices");
