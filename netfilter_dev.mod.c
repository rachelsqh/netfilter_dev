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
	{ 0x9463ffe0, "module_layout" },
	{ 0x5b0ea73c, "device_destroy" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xe0285ccb, "class_destroy" },
	{ 0x28a97f1a, "device_create" },
	{ 0x32b8c91b, "__class_create" },
	{ 0xc5850110, "printk" },
	{ 0x26d8100e, "__register_chrdev" },
	{ 0xfc968d49, "nf_register_net_hook" },
	{ 0x5e050eaa, "kmem_cache_alloc_trace" },
	{ 0xcfd50321, "kmalloc_caches" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x2fd498bc, "skb_copy" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x278dd787, "skb_dequeue" },
	{ 0x37a0cba, "kfree" },
	{ 0xf3f0ad9a, "nf_unregister_net_hook" },
	{ 0x714bd61f, "init_net" },
	{ 0xc29957c3, "__x86_indirect_thunk_rcx" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

