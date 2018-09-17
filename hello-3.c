//  Nome e RA dos integrantes do grupo
//Nome: Breno Baldovinotti           RA:14315311
//Nome: Gabriela Ferreira Jorge      RA:12228441
//Nome: Marco Antônio de Nadai Filho RA:16245961
//Nome: Moacyr Silva Neto            RA:15191372
//Nome: Nícolas Leonardo Kupka       RA:16104325

/*
* hello-3.c - Illustrating the __init, __initdata and __exit macros.
*/
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */

static int hello3_data __initdata = 3;

static int __init hello_3_init(void)
{
    pr_info("Hello, world %d\n", hello3_data);
    return 0;
}

static void __exit hello_3_exit(void)
{
    pr_info("Goodbye, world 3\n");
}

module_init(hello_3_init);
module_exit(hello_3_exit);