#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xf28efea0, "module_layout" },
	{ 0x53934f07, "platform_driver_unregister" },
	{ 0xb3e308df, "__platform_driver_register" },
	{ 0xbaaced3b, "_dev_info" },
	{ 0x26cbdb59, "_dev_err" },
	{ 0xf4444dbc, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x85fd2b95, "of_get_named_gpio_flags" },
	{ 0xc5850110, "printk" },
	{ 0xfe990052, "gpio_free" },
	{ 0xdd4e19e2, "gpiod_set_raw_value" },
	{ 0xb8662a86, "gpio_to_desc" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cmy,gpio-led");
MODULE_ALIAS("of:N*T*Cmy,gpio-ledC*");
