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
	{ 0x102fe6de, "hrtimer_cancel" },
	{ 0x37a0cba, "kfree" },
	{ 0x5ec9a568, "device_remove_file" },
	{ 0x94090688, "misc_deregister" },
	{ 0x9f1dc8c6, "kmalloc_caches" },
	{ 0xc4a913aa, "__kmalloc_cache_noprof" },
	{ 0x2002cbd1, "misc_register" },
	{ 0x5aa1431d, "device_create_file" },
	{ 0xea82d349, "hrtimer_init" },
	{ 0xc0b7c197, "hrtimer_start_range_ns" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0xfff5afc, "time64_to_tm" },
	{ 0xaafdc258, "strcasecmp" },
	{ 0x96848186, "scnprintf" },
	{ 0x135bb7ec, "hrtimer_forward" },
	{ 0xd36dc10c, "get_random_u32" },
	{ 0x43024713, "device_show_int" },
	{ 0xaae150ee, "device_store_int" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "ED3F934EB4BC13590BA8F17");
