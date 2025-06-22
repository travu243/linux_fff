#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/property.h>

static struct gpio_desc *led_gpio;
static struct gpio_desc *btn_gpio;
static int irq_number;
static bool led_state = false;
static const char *led_name = "unknown";
static const char *button_name = "unknown";

static irqreturn_t button_isr(int irq, void *dev_id)
{
    led_state = !led_state;
    gpiod_set_value(led_gpio, led_state);
    pr_info("%s toggled %s\n", button_name, led_state ? "ON" : "OFF");
    return IRQ_HANDLED;
}

static int btn_led_probe(struct platform_device *pdev)
{
    struct fwnode_handle *fwnode = dev_fwnode(&pdev->dev);
    int ret;

    // read string from device tree
    device_property_read_string(&pdev->dev, "led-name", &led_name);
    device_property_read_string(&pdev->dev, "button-name", &button_name);

    // take GPIO from devm API
    led_gpio = devm_gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        dev_err(&pdev->dev, "failed to get LED GPIO\n");
        return PTR_ERR(led_gpio);
    }

    btn_gpio = devm_gpiod_get_optional(&pdev->dev, NULL, GPIOD_IN);
    if (IS_ERR(btn_gpio)) {
        dev_err(&pdev->dev, "failed to get Button GPIO\n");
        return PTR_ERR(btn_gpio);
    }

    // take IRQ from button GPIO
    irq_number = gpiod_to_irq(btn_gpio);
    if (irq_number < 0) {
        dev_err(&pdev->dev, "failed to get IRQ\n");
        return irq_number;
    }

    // sign up IRQ
    ret = devm_request_irq(&pdev->dev, irq_number, button_isr,
                           IRQF_TRIGGER_FALLING | IRQF_SHARED,
                           "gpio_button_irq", &pdev->dev);
    if (ret) {
        dev_err(&pdev->dev, "failed to request IRQ\n");
        return ret;
    }

    dev_info(&pdev->dev, "probed: led='%s', button='%s'\n", led_name, button_name);
    return 0;
}

static int btn_led_remove(struct platform_device *pdev)
{
    pr_info("led button removed\n");
    return 0;
}

static const struct of_device_id btn_led_of_match[] = {
    { .compatible = "my,gpio-led" },  // match node root
    { }
};
MODULE_DEVICE_TABLE(of, btn_led_of_match);

static struct platform_driver btn_led_driver = {
    .probe  = btn_led_probe,
    .remove = btn_led_remove,
    .driver = {
        .name = "led_button_fwnode",
        .of_match_table = btn_led_of_match,
    },
};
module_platform_driver(btn_led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dangcap");
MODULE_DESCRIPTION("led button");
