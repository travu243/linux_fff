#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/io.h>

static int mydevice_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    u32 int_val, array_val[4], child_int;
    const char *str_val, *child_str, *override_val;
    int len, i;
    struct device_node *child;
    const __be32 *reg_prop;

    dev_info(&pdev->dev, "probing main device...\n");

    // main node: int
    if (!of_property_read_u32(np, "my-int-prop", &int_val))
        dev_info(&pdev->dev, "my-int-prop = %u\n", int_val);

    // main node: string
    if (!of_property_read_string(np, "my-string-prop", &str_val))
        dev_info(&pdev->dev, "my-string-prop = %s\n", str_val);

    // main node: array
    len = of_property_count_u32_elems(np, "my-array-prop");
    if (len > 0 && len <= 4) {
        of_property_read_u32_array(np, "my-array-prop", array_val, len);
        dev_info(&pdev->dev, "my-array-prop:");
        for (i = 0; i < len; i++)
            dev_info(&pdev->dev, "  [%d] = %u", i, array_val[i]);
    }

    // child node
    child = of_get_child_by_name(np, "child_device");
    if (!child) {
        dev_err(&pdev->dev, "Child node not found\n");
        return -ENODEV;
    }

    // read child int
    if (!of_property_read_u32(child, "child-int", &child_int))
        dev_info(&pdev->dev, "child-int = %u\n", child_int);

    // read child string
    if (!of_property_read_string(child, "child-string", &child_str))
        dev_info(&pdev->dev, "child-string = %s\n", child_str);

    // read reg
    reg_prop = of_get_property(child, "reg", &len);
    if (reg_prop && len >= sizeof(u32) * 2) {
        dev_info(&pdev->dev, "child reg base = 0x%x, size = 0x%x",
                 be32_to_cpup(reg_prop),
                 be32_to_cpup(reg_prop + 1));
    }

    // read overridden property
    if (!of_property_read_string(child, "override-this", &override_val))
        dev_info(&pdev->dev, "override-this = %s\n", override_val);

    return 0;
}

static int mydevice_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "removing main device\n");
    return 0;
}

static const struct of_device_id mydevice_dt_ids[] = {
    { .compatible = "travu,ex2" },
    { }
};
MODULE_DEVICE_TABLE(of, mydevice_dt_ids);

static struct platform_driver mydevice_driver = {
    .driver = {
        .name = "mydevice_driver",
        .of_match_table = mydevice_dt_ids,
    },
    .probe = mydevice_probe,
    .remove = mydevice_remove,
};
module_platform_driver(mydevice_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("travu");
MODULE_DESCRIPTION("ex2");
