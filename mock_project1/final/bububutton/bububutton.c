#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/io.h>

// from ledledled.c export
extern void set_led_state(bool a);

static struct {
    int gpio;
    int irq;
    bool led_state;
} button_data;

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
    button_data.led_state = !button_data.led_state;
    set_led_state(button_data.led_state);
    //pr_info("bububutton: led: %d\n", button_data.led_state);
    return IRQ_HANDLED;
}

static int bububutton_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    if (of_device_is_compatible(dev->of_node, "tra,gpio-button")){
        button_data.gpio = of_get_named_gpio(dev->of_node, "gpios", 0);
        if (!gpio_is_valid(button_data.gpio)) {
            dev_err(dev, "invalid gpio\n");
            return -EINVAL;
        }

        gpio_request(button_data.gpio, "btn_gpio");
        gpio_direction_input(button_data.gpio);

        button_data.irq = gpio_to_irq(button_data.gpio);
        if (request_threaded_irq(button_data.irq, NULL, button_irq_handler,
                                  IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                                  "bububutton_irq", NULL)) {
            dev_err(dev, "irq fail\n");
            return -EINVAL;
        }

        pr_info("bububutton gpio %d irq %d registered\n", button_data.gpio, button_data.irq);

    }

    return 0;
}

static void bububutton_remove(struct platform_device *pdev)
{
    free_irq(button_data.irq, NULL);
    gpio_free(button_data.gpio);
    pr_info("bububutton: removed\n");
}

static const struct of_device_id bububutton_of_match[] = {
    { .compatible = "tra,gpio-button" },
    {},
};
MODULE_DEVICE_TABLE(of, bububutton_of_match);

static struct platform_driver bububutton_driver = {
    .probe = bububutton_probe,
    .remove = bububutton_remove,
    .driver = {
        .name = "bububutton_driver",
        .of_match_table = bububutton_of_match,
    }
};

module_platform_driver(bububutton_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("BUBUBUTTON");
