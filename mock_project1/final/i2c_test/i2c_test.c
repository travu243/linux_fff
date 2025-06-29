#include <linux/module.h>       // MODULE_LICENSE, MODULE_AUTHOR, module_init/module_exit
#include <linux/init.h>         // __init, __exit
#include <linux/i2c.h>          // i2c_adapter,struct i2c_msg,i2c_get_adapter,i2c_transfer,i2c_put_adapter
#include <linux/kthread.h>      // kthread_run(), kthread_stop(), kthread_should_stop(), struct task_struct
#include <linux/delay.h>        // msleep()
#include <linux/fs.h>           // alloc_chrdev_region(), struct file_operations
#include <linux/uaccess.h>      // copy_to_user()
#include <linux/cdev.h>         // struct cdev, cdev_init(), cdev_add(), cdev_del()
#include <linux/device.h>       // class_create(), device_create(), device_destroy(), class_destroy()
#include <linux/ioctl.h>        // _IOW, _IOR macros for ioctl

#define DRIVER_NAME     "i2c_pcf_driver"
#define DEVICE_NAME     "i2c_ledchar"
#define CLASS_NAME      "i2c_class"
#define SLAVE_ADDR      0x27
#define I2C_BUS_NUM     2
#define BUF_SIZE        4

#define I2C_GET_VALUE _IOR(0x99, 0, int)  // <-- Macro ioctl

static struct task_struct *sender_task;
static struct i2c_adapter *adapter;
static int last_sent_value;

static dev_t dev_num;
static struct cdev i2c_cdev;
static struct class *i2c_class;

static int sender_thread_fn(void *data) {
    u8 value = 0;
    struct i2c_msg msg;
    u8 buf[1];

    while (!kthread_should_stop()) {
        buf[0] = value;

        msg.addr = SLAVE_ADDR;
        msg.flags = 0;
        msg.len = 1;
        msg.buf = buf;

        if (i2c_transfer(adapter, &msg, 1) != 1) {
            pr_err("%s: i2c_transfer failed at value = %u\n", DRIVER_NAME, value);
        } else {
            last_sent_value = value;
            //pr_info("%s: Sent 0x%02X\n", DRIVER_NAME, value);
        }

        value = (value + 1) % 8;
        msleep(1000);
    }

    return 0;
}

// ioctl
static long i2c_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case I2C_GET_VALUE:
        if (copy_to_user((int __user *)arg, &last_sent_value, sizeof(int)))
            return -EFAULT;
        return 0;
    default:
        return -ENOTTY;
    }
}

// read
static ssize_t i2c_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[BUF_SIZE];
    int len;

    if (*ppos > 0) return 0;

    len = snprintf(kbuf, BUF_SIZE, "%d\n", last_sent_value);

    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    *ppos = len;
    return len;
}

// file_operations
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = i2c_read,
    .unlocked_ioctl = i2c_ioctl,
};

static int __init i2c_api_sender_init(void) {
    pr_info("%s: init\n", DRIVER_NAME);

    adapter = i2c_get_adapter(I2C_BUS_NUM);
    if (!adapter) {
        pr_err("%s: Failed to get i2c adapter %d\n", DRIVER_NAME, I2C_BUS_NUM);
        return -ENODEV;
    }

    // character device init
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&i2c_cdev, &fops);
    cdev_add(&i2c_cdev, dev_num, 1);
    i2c_class = class_create(CLASS_NAME);
    device_create(i2c_class, NULL, dev_num, NULL, DEVICE_NAME);
    pr_info("%s: /dev/%s created\n", DRIVER_NAME, DEVICE_NAME);

    // start i2c sending thread
    sender_task = kthread_run(sender_thread_fn, NULL, "i2c_sender_thread");
    if (IS_ERR(sender_task)) {
        device_destroy(i2c_class, dev_num);
        class_destroy(i2c_class);
        cdev_del(&i2c_cdev);
        unregister_chrdev_region(dev_num, 1);
        i2c_put_adapter(adapter);
        pr_err("%s: Failed to create thread\n", DRIVER_NAME);
        return PTR_ERR(sender_task);
    }

    return 0;
}

static void __exit i2c_api_sender_exit(void) {
    pr_info("%s: exit\n", DRIVER_NAME);

    if (sender_task)
        kthread_stop(sender_task);

    if (adapter)
        i2c_put_adapter(adapter);

    device_destroy(i2c_class, dev_num);
    class_destroy(i2c_class);
    cdev_del(&i2c_cdev);
    unregister_chrdev_region(dev_num, 1);
}

late_initcall_sync(i2c_api_sender_init);//call after i2c inited
module_exit(i2c_api_sender_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("I2C pcf8574 char device");
