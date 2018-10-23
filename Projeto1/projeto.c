#include <linux/init.h>      
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>        
#include <linux/fs.h>          
#include <asm/uaccess.h>          
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <crypto/skcipher.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <crypto/hash.h>



#define  DEVICE_NAME "sob"          //< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "Crypto"       //< The device class -- this is a character device driver

static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
int gerarHash(char *dados);
static int mocacp_skcipher(char *word, int operation);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moacyr e companhia LTDA");

MODULE_DESCRIPTION("SOB Project");
MODULE_VERSION("1.0");

static char *key = "default";
static int moduleMajorVersion;
static struct class*  moduleClass  = NULL; ///< The device-driver class struct pointer
static struct device* moduleDevice = NULL; ///< The device-driver device struct pointer
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;       
/* 
 * module_param(key, charp, 0000)
 * The first param is the parameters name
 * The second param is it's data type
 * The final argument is the permissions bits, 
 * for exposing parameters in sysfs (if non-zero) at a later stage.
 */

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "A key string");

static struct file_operations operacoes = 
{
  .read = dev_read,
  .write = dev_write,
};

// THIS_MODULE: Is an macro from module.h
// IS_ERR: Tests if the supplied pointer should be considered an error value instead of a valid pointer to data.

//############# register_chrdev #############
//If major == 0 this functions will dynamically allocate a major and return its number.
//If major > 0 this function will attempt to reserve a device with the given major number and will return zero on success.
//Returns a -ve errno on failure.
//The name of this device has nothing to do with the name of the device in /dev. It only helps to keep track of the different owners of devices. If your module name has only one type of devices it's ok to use e.g. the name of the module here.
//This function registers a range of 256 minor numbers. The first minor number is 0.

//############# unregister_chrdev #############
// Unregister and destroy the cdev occupying the region described by major, baseminor and count. This function undoes what __register_chrdev did.

//############# class_create #############
// This is used to create a struct class pointer that can then be used in calls to class_device_create.
// Note, the pointer created here is to be destroyed when finished by making a call to class_destroy.

//############# device_create #############
// This function can be used by char device classes. A struct device will be created in sysfs, registered to the specified class.
// the struct class passed to this function must have previously been created with a call to class_create.

static int __init init(void)
{
    printk(KERN_INFO "Loading driver! \n");

    moduleMajorVersion = register_chrdev(0, DEVICE_NAME, &operacoes);

    if (moduleMajorVersion<0)
    {
        printk(KERN_ALERT "Device failed to receive a version\n");
        return moduleMajorVersion;
    }
    printk(KERN_INFO "Device succeeded to receive a version!\n", moduleMajorVersion);

    moduleClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(moduleClass))
    {
        unregister_chrdev(moduleMajorVersion, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create a class fot the module\n");
        return PTR_ERR(moduleClass);
    }
    printk(KERN_INFO "Module class created!\n");

    moduleDevice = device_create(moduleClass, NULL, MKDEV(moduleMajorVersion, 0), NULL, DEVICE_NAME);
    if (IS_ERR(moduleDevice))
    { 
        class_destroy(moduleClass);
        unregister_chrdev(moduleMajorVersion, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device\n");
        return PTR_ERR(moduleDevice);
    }
    printk(KERN_INFO "Device created with success\n");

    printk(KERN_INFO "HASH KEY: %s\n",key);

    return 0;
}

//############# device_destroy #############
// This call unregisters and cleans up a device that was created with a call to device_create.

//############# class_unregister #############
// unregister device class

//############# class_destroy #############
// pointer to the struct class that is to be destroyed
// Note, the pointer to be destroyed must have been created with a call to class_create.

//############# unregister_chrdev #############
// Unregister and destroy the cdev occupying the region described by major, baseminor and count. This function undoes what __register_chrdev did.



static void __exit exit(void)
{
    device_destroy(moduleClass, MKDEV(moduleMajorVersion, 0));
    class_unregister(moduleClass);
    class_destroy(moduleClass);
    unregister_chrdev(moduleMajorVersion, DEVICE_NAME);
    printk(KERN_INFO "Removing driver\n");
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    char operation, space, word[len-2];
    
    operation = buffer[0];

    space = buffer[1];

    strncpy(word, buffer+2, sizeof(len-2));
    
    if (space != ' ') {
        printk(KERN_INFO "Fail to parse: %s\n", buffer);
        return 0;
    }

    typecheck(word,16);


    switch (operation) {
        case 'c': // 1 para cifragem
            printk(KERN_INFO "Option ----------------------> C");
            printk(KERN_INFO "Message ---------------------> %s\n", buffer);
            mocacp_skcipher(word,1,key);
            break;
        case 'd': // 0 para decifragem
            printk(KERN_INFO "Option ----------------------> D");
            printk(KERN_INFO "Message ---------------------> %s\n", buffer);
            mocacp_skcipher(word,0,key);
            break;
        case 'h':
            printk(KERN_INFO "Option ----------------------> h");
            printk(KERN_INFO "Message ---------------------> %s\n", buffer);
            return gerarHash(word);
            break;
        default:
            printk(KERN_INFO "Error");
            printk(KERN_INFO "Message ---------------------> %s\n", buffer);
            break;
    }

    return len;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    int error_count = 0;
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count==0){            // if true then have success
        printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
        return (size_of_message=0);  // clear the position to the start and return 0
    }
    else {
        printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }
}

int gerarHash(char *dados){

    struct crypto_shash *tfm;
    struct shash_desc *desc;
    
    unsigned char digest[32];
    
    unsigned int shash_desc_size;
    int i;
    int ret;

    tfm = crypto_alloc_shash("sha256", 0, CRYPTO_ALG_ASYNC);

    shash_desc_size = sizeof(struct shash_desc) + crypto_shash_descsize(tfm);

    desc = kmalloc(shash_desc_size, GFP_KERNEL);


    desc->tfm = tfm;
    desc->flags = 0;

    ret = crypto_shash_digest(desc, dados, strlen(dados), digest);

    printk(KERN_INFO "Hash:");
    
    for(i = 0; i <= 32; i++)
    {
        printk(KERN_INFO "%02x", digest[i]);
    }

    return ret;
}



// struct com o resultado da cifragem/decifragem do conteúdo enviado
struct tcrypt_result {
    struct completion completion;
    int err;
};

// struct que define o cipher que utilizaremos
struct skcipher_def {
    struct scatterlist sg;
    struct crypto_skcipher *tfm; // transformation é o tipo de algoritmo que usaremos, no caso ecb(aes)
    struct skcipher_request *req;// qual a função requisitada pelo user, c para cifrar, d para decifrar
    struct tcrypt_result result; // struct com resultado da cifra/decifra
};

/* Callback function */ // função usada para verificar se cifra/decifra está pronta para retornar ao module
static void mocacp_skcipher_cb(struct crypto_async_request *req, int error)
{
    struct tcrypt_result *result = req->data;

    if (error == -EINPROGRESS) // Cifragem ou decifragem em progresso
        return;
    result->err = error;
    complete(&result->completion);
    pr_info("Encryption finished successfully\n");
}

/* Perform cipher operation */  //a partir da escolha do user, cifrar ou decifrar o conteúdo 
static unsigned int mocacp_skcipher_encdec(struct skcipher_def *sk,
                     int enc) //int enc é a opção escolhida
{
    int rc = 0;

    if (enc)
        rc = crypto_skcipher_encrypt(sk->req);
    else
        rc = crypto_skcipher_decrypt(sk->req);

    switch (rc) {
    case 0:
        break;
    case -EINPROGRESS:
    case -EBUSY:
        rc = wait_for_completion_interruptible(
            &sk->result.completion);
        if (!rc && !sk->result.err) {
            reinit_completion(&sk->result.completion);
            break;
        }
    default:
        pr_info("skcipher encrypt returned with %d result %d\n",
            rc, sk->result.err);
        break;
    }
    init_completion(&sk->result.completion);

    return rc;
}

/*static int hexBlockConvert(char *word){
    char dataChar = word;
    int strignLength = strlen(dataChar);
}*/

/* Initialize and trigger cipher operation */ // função utilizada no module para triggar a cifra/decifra
static int mocacp_skcipher(char *word, int operation, char *key)   // esta é a função principal de utilização do Kernel Crypto API, que chama todas as func e structs necessárias para cifrar/decifrar, dependendo da opção escolhida pelo user
{  
    struct skcipher_def sk;
    struct crypto_skcipher *skcipher = NULL;
    struct skcipher_request *req = NULL;
    int ret = -EFAULT;

    skcipher = crypto_alloc_skcipher("ecb(aes)", 0, 0); //usar int crypto_has_skcipher() para encontrar se existe ecb(aes), não utilizado pois foi verificado no arquivo /proc/crypto que o driver com esse nome existe
    if (IS_ERR(skcipher)) {
        pr_info("Could not allocate skcipher handle\n");
        return PTR_ERR(skcipher);
    }

    req = skcipher_request_alloc(skcipher, GFP_KERNEL); //requisição do cipher handle
    if (!req) {
        pr_info("Could not allocate skcipher request\n");
        ret = -ENOMEM;
        goto out;
    }

    skcipher_request_set_callback(req, 0,
                      mocacp_skcipher_cb,
                      &message);

    /* AES 256 */ //usando key dada pelo user
    if (crypto_skcipher_setkey(skcipher, key, 32)) {
        pr_info("key could not be set\n");
        ret = -EAGAIN;
        goto out;
    }

    sk.tfm = skcipher;
    sk.req = req;

    // transformar bloco de dados em hexa, antes de cifrar/decifrar
    // hexBlockConvert();

    /* We encrypt one block */ //Deve ser alterado para que o bloco de dados seja em hexa (ou seja, base 16)
    sg_init_one(&sk.sg, &word, 16);
    skcipher_request_set_crypt(req, &sk.sg, &sk.sg, 16, NULL);
    init_completion(&sk.result.completion);

    /* encrypt data */ //para que escolha de acordo com user, segundo param deve ser a váriavel 
    ret = mocacp_skcipher_encdec(&sk, operation); //chama a operação de cifra/decifra, e executa a operação requisitada
    if (ret)
        goto out;

    pr_info("Encryption triggered successfully\n");

out:
    if (skcipher)
        crypto_free_skcipher(skcipher); //zera e libera cipher handle
    if (req)
        skcipher_request_free(req); //zera e libera a struct de requisição de cifra/decifra
    return ret;
}



module_init(init);
module_exit(exit);


