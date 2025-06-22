#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#define GPIO0_ADDR_BASE           0x44E07000
#define GPIO0_ADDR_END            0x44E07FFF
#define GPIO0_ADDR_SIZE           (GPIO0_ADDR_END - GPIO0_ADDR_BASE + 1)

#define GPIO_OE_OFFSET            0x134
#define GPIO_CLEARDATAOUT_OFFSET  0x190
#define GPIO_SETDATAOUT_OFFSET    0x194

#define GPIO0_30                  (1 << 30)  // P9_11

static void __io	mem *gpio0_base = NULL;

static int __init led_gpio_init(void)
{
    u32 val;

    pr_info("led init\n");

    gpio0_base = ioremap(GPIO0_ADDR_BASE, GPIO0_ADDR_SIZE);
    if (!gpio0_base) {
        pr_err("Failed to ioremap GPIO0 registers\n");
        return -ENOMEM;
    }

    // Set GPIO0_30 as output (clear bit 30 in OE register)
    val = ioread32(gpio0_base + GPIO_OE_OFFSET);
    val &= ~GPIO0_30;
    iowrite32(val, gpio0_base + GPIO_OE_OFFSET);

    // set GPIO0_30 = 1
    iowrite32(GPIO0_30, gpio0_base + GPIO_SETDATAOUT_OFFSET);
    pr_info("GPIO0_30 (P9_11) set HIGH\n");

    return 0;
}

static void __exit led_gpio_exit(void)
{
    pr_info("exit\n");

    if (gpio0_base) {
        // set GPIO0_30 = 0
        iowrite32(GPIO0_30, gpio0_base + GPIO_CLEARDATAOUT_OFFSET);
        iounmap(gpio0_base);
    }

    pr_info("GPIO0_30 (P9_11) set 0\n");
}

module_init(led_gpio_init);
module_exit(led_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dangcap");
MODULE_DESCRIPTION("led");
