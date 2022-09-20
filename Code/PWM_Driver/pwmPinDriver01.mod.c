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
	{ 0x70035c2e, "module_layout" },
	{ 0xcb369c55, "cdev_del" },
	{ 0xedc03953, "iounmap" },
	{ 0x1d37eeed, "ioremap" },
	{ 0x7d9b3fc1, "class_destroy" },
	{ 0xe162751b, "device_destroy" },
	{ 0x824de4cd, "cdev_add" },
	{ 0x403dda38, "cdev_init" },
	{ 0x74488b62, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x831e1955, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xc5850110, "printk" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "959BD5F4AB523E0DC0B2159");
