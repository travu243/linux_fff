#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int my_sensor_probe(struct platform_device *pdev)
{
    u32 threshold;

    if (of_property_read_u32(pdev->dev.of_node, "threshold", &threshold)) {
        dev_err(&pdev->dev, "Cannot read threshold property\n");
        return -EINVAL;
    }

    dev_info(&pdev->dev, "Probed with threshold = %u\n", threshold);
    return 0;
}

static int my_sensor_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Removed\n");
    return 0;
}

static const struct of_device_id my_sensor_of_match[] = {
    { .compatible = "my-sensor", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_sensor_of_match);

static struct platform_driver my_sensor_driver = {
    .probe  = my_sensor_probe,
    .remove = my_sensor_remove,
    .driver = {
        .name = "my_sensor",
        .of_match_table = my_sensor_of_match,
    },
};
module_platform_driver(my_sensor_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dangcap");
MODULE_DESCRIPTION("my-sensor");
