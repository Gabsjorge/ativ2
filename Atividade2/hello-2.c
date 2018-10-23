//  Nome e RA dos integrantes do grupo
//Nome: Breno Baldovinotti           RA:14315311
//Nome: Gabriela Ferreira Jorge      RA:12228441
//Nome: Marco Antônio de Nadai Filho RA:16245961
//Nome: Moacyr Silva Neto            RA:15191372
//Nome: Nícolas Leonardo Kupka       RA:16104325

/*  
 *  hello-2.c - Demonstrating the module_init() and module_exit() macros.
 *  This is preferred over using init_module() and cleanup_module().
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

static int __init hello_2_init(void)
{
	printk(KERN_INFO "Hello, world 2\n");
	return 0;
}

static void __exit hello_2_exit(void)
{
	printk(KERN_INFO "Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);