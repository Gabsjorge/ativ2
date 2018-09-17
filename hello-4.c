//  Nome e RA dos integrantes do grupo
//Nome: Breno Baldovinotti           RA:14315311
//Nome: Gabriela Ferreira Jorge      RA:12228441
//Nome: Marco Antônio de Nadai Filho RA:16245961
//Nome: Moacyr Silva Neto            RA:15191372
//Nome: Nícolas Leonardo Kupka       RA:16104325

/*
* hello-4.c - Demonstrates module documentation.
*/
#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/init.h>      /* Needed for the macros */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bob Mottram and Gabriela Jorge");
MODULE_DESCRIPTION("A sample driver");
MODULE_SUPPORTED_DEVICE("testdevice");

static int __init init_hello_4(void)
{
    pr_info("Hello, world 4\n");
    return 0;
}

static void __exit cleanup_hello_4(void)
{
    pr_info("Goodbye, world 4\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);