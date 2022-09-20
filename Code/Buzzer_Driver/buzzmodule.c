#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/io.h>

#include "utils.h"

#define DEVICE_NAME "buzzer0"
#define CLASS_NAME "buzzerClass"

/*These modules are written under the GNU Public License (GPL) so that third party users are warned that this code is non open-source.*/
MODULE_LICENSE("GPL");
/*This macro call is inserted manually to trick the kernel*/
MODULE_INFO(intree, "Y");

/* Device variables */
struct class* buzzDevice_class = NULL; //class pointer
//static struct device* buzzDevice_device = NULL;
static dev_t buzzDevice_majorminor; //device
static struct cdev c_dev;  // Character device structure

/*static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;*/
struct GpioRegisters *s_pGpioRegisters;

/*Buzzer pin -> GPIO 23 [PIN 16]*/
static const int buzzGpioPin = 23;

/**
 * function to write on buzzer device driver
 * @return len
 */
ssize_t buzz_device_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) {
	struct GpioRegisters *pdev; 
		
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GpioRegisters *)pfile->private_data;
	if (pbuff[0]=='0')
		SetGPIOOutputValue(pdev, buzzGpioPin, 0);
	else
		SetGPIOOutputValue(pdev, buzzGpioPin, 1);
	return len;
}

/**
 * function to read of buzzer device driver (not used)
 * @return len
 */
ssize_t buzz_device_read(struct file *pfile, char __user *p_buff,size_t len, loff_t *poffset){
	pr_alert("%s: called (%u)\n",__FUNCTION__,len);
	return 0;
}

/**
 * function to close the buzzer device driver
 * @return 0
 */
int buzz_device_close(struct inode *p_inode, struct file * pfile){
	pfile->private_data = NULL;
	return 0;
}

/**
 * function to open the buzzer device driver
 * @return 0
 */
int buzz_device_open(struct inode* p_indode, struct file *p_file){

	pr_alert("%s: called\n",__FUNCTION__);
	/**< private_data is a useful resource for preserving state information across system calls;*/
	p_file->private_data = (struct GpioRegisters *) s_pGpioRegisters;
	return 0;
	
}

/**< file operations structure -
 * The file structure represents an open file. It is created by the kernel
 * on open and is passed to any function that operates on the file,
 * until the last close.
 * */
static struct file_operations buzzDevice_fops = {
	.owner = THIS_MODULE,
	.write = buzz_device_write,
	.read = buzz_device_read,
	.release = buzz_device_close,
	.open = buzz_device_open,
};

/**
 * Init fucntion
 * @return 0
 */
static int __init buzzModule_init(void) {
	int ret;
	struct device *dev_ret;


	/**< Allocatesa range of char device numbers. The major number will be chosen dynamically, and returned */
	if ((ret = alloc_chrdev_region(&buzzDevice_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	/**< IS_ERR is used to check if class_create succeded creating ledDevice_class. If an error occurs unregister 
	 * the char driver and signal the error. */
	if (IS_ERR(buzzDevice_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(buzzDevice_majorminor, 1);
		return PTR_ERR(buzzDevice_class);
	}

	/**< Creates a device and registers it.*/
	if (IS_ERR(dev_ret = device_create(buzzDevice_class, NULL, buzzDevice_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(buzzDevice_class);
		unregister_chrdev_region(buzzDevice_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	/**< Creates (initializes) a character device */
	cdev_init(&c_dev, &buzzDevice_fops);
	c_dev.owner = THIS_MODULE;
	/**< Add a char device to the system */
	if ((ret = cdev_add(&c_dev, buzzDevice_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(buzzDevice_class, buzzDevice_majorminor);
		class_destroy(buzzDevice_class);
		unregister_chrdev_region(buzzDevice_majorminor, 1);
		return ret;
	}

	/**< map bus memory into CPU space */
	s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));
	//s_pGpioRegisters = (struct GpioRegisters *)ioremap_nocache(GPIO_BASE, sizeof(struct GpioRegisters));
	
	pr_alert("map to virtual address: 0x%x\n", (unsigned)s_pGpioRegisters);
	
	SetGPIOFunction(s_pGpioRegisters, buzzGpioPin, 0b001); //Output

	return 0;
}

/**
 * Exit fuction
 * @return 0
 */
static void __exit buzzModule_exit(void) {
	SetGPIOFunction(s_pGpioRegisters, buzzGpioPin, 0); //Configure the pin as input
	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(buzzDevice_class, buzzDevice_majorminor);
	class_destroy(buzzDevice_class);
	unregister_chrdev_region(buzzDevice_majorminor, 1);
}

module_init(buzzModule_init);
module_exit(buzzModule_exit);
