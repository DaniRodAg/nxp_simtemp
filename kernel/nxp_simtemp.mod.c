#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x9166fada, "strncpy" },
	{ 0x1ac5d3cb, "strcspn" },
	{ 0xe914e41e, "strcpy" },
	{ 0x98cf60b3, "strlen" },
	{ 0x122c3a7e, "_printk" },
	{ 0xfa474811, "__platform_driver_register" },
	{ 0x102fe6de, "hrtimer_cancel" },
	{ 0x5ec9a568, "device_remove_file" },
	{ 0x94090688, "misc_deregister" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0xfff5afc, "time64_to_tm" },
	{ 0xaafdc258, "strcasecmp" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0x96848186, "scnprintf" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0xe2964344, "__wake_up" },
	{ 0xd36dc10c, "get_random_u32" },
	{ 0x80192c9b, "of_property_read_variable_u32_array" },
	{ 0x637962ed, "_dev_info" },
	{ 0x2002cbd1, "misc_register" },
	{ 0x5aa1431d, "device_create_file" },
	{ 0xea82d349, "hrtimer_init" },
	{ 0xc0b7c197, "hrtimer_start_range_ns" },
	{ 0x9d1f3a80, "_dev_warn" },
	{ 0x61fd46a9, "platform_driver_unregister" },
	{ 0xdcb764ad, "memset" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x135bb7ec, "hrtimer_forward" },
	{ 0x43024713, "device_show_int" },
	{ 0xaae150ee, "device_store_int" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cnxp,simtemp");
MODULE_ALIAS("of:N*T*Cnxp,simtempC*");

MODULE_INFO(srcversion, "7B1AF906AF2A6B3D86134B4");
