#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>

static int led_gpio = -1;

static int gpio_led_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    int ret;

    led_gpio = of_get_named_gpio(dev->of_node, "gpios", 0);
    if (!gpio_is_valid(led_gpio)) {
        dev_err(dev, "invalid GPIO from dt\n");
        return -EINVAL;
    }

    ret = gpio_request(led_gpio, "gpio_led");
    if (ret) {
        dev_err(dev, "failed to request GPIO %d\n", led_gpio);
        return ret;
    }

    ret = gpio_direction_output(led_gpio, 1); // led on
    if (ret) {
        dev_err(dev, "failed to set direction\n");
        gpio_free(led_gpio);
        return ret;
    }

    dev_info(dev, "led %d set 1\n", led_gpio);
    return 0;
}

static int gpio_led_remove(struct platform_device *pdev)
{
    if (gpio_is_valid(led_gpio)) {
        gpio_set_value(led_gpio, 0); // turn off
        gpio_free(led_gpio);
        pr_info("LED GPIO %d set 0 and freed\n", led_gpio);
    }
    return 0;
}

static const struct of_device_id gpio_led_dt_ids[] = {
    { .compatible = "my,gpio-led", },
    { }
};
MODULE_DEVICE_TABLE(of, gpio_led_dt_ids);

static struct platform_driver gpio_led_driver = {
    .probe  = gpio_led_probe,
    .remove = gpio_led_remove,
    .driver = {
        .name = "gpio_led_dt_driver",
        .of_match_table = gpio_led_dt_ids,
    },
};
module_platform_driver(gpio_led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dangcap");
MODULE_DESCRIPTION("led device tree");
