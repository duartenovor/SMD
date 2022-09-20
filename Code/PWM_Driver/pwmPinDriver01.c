/**< BCM2835-ARM-Peripherals.pdf document:
 *  https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");

/* GPIO registers base address. */
#define BCM2711_PERI_BASE   (0xFE000000)
#define GPIO_BASE           (BCM2711_PERI_BASE + 0x200000)

#define GPIO_18 18 //GPIO 18 (PIN 12)
#define GPIO_19 19 //GPIO 19 (PIN 35)

/**< Oscilator = 54MHz
 * Clock divider is set to 16.
 * 54MHz/16 = 3.375MHz
 * With a divider of 16 and a objetctive of 50Hz, in MARKSPACE mode,
 * the pulse repetition frequency will be
 * 3.375MHz/50Hz = 67500, suitable for driving the servo motor with PWM 
*/
#define CLK_DIVISOR 16
#define RANGE       67500

/**< 
 * SIZE is 2^16-1 = 65535
 * 65536-20ms <=> [UP]-2ms -> 6554
 * 65536-20ms <=> [MIDDLE]-1.5ms -> 6554
 * 65536-20ms <=> [DOWN]-1ms -> 6554
 * */
#define UP          6554    /**< 2ms */
#define MIDDLE      4915    /**< 1.5ms */
#define DOWN        3277    /**< 1ms */

/* GPIO registers [PAGE 90 -> BC2835 ARM PERIPHERALS][TABLE 6-1]*/
struct S_GPIO_REGS
{
      uint32_t GPFSEL[6];
      uint32_t Reserved0;
      uint32_t GPSET[2];
      uint32_t Reserved1;
      uint32_t GPCLR[2];
      uint32_t Reserved2;
      uint32_t GPLEV[2];
      uint32_t Reserved3;
      uint32_t GPEDS[2];
      uint32_t Reserved4;
      uint32_t GPREN[2];
      uint32_t Reserved5;
      uint32_t GPFEN[2];
      uint32_t Reserved6;
      uint32_t GPHEN[2];
      uint32_t Reserved7;
      uint32_t GPLEN[2];
      uint32_t Reserved8;
      uint32_t GPAREN[2];
      uint32_t Reserved9;
      uint32_t GPAFEN[2];
      uint32_t Reserved10;
      uint32_t GPPUD;
      uint32_t GPPUDCLK[2];
      uint32_t Reserved11[4];
} *gpio_regs;

/* GPIO Pin Alternative Function selection */
// By default GPIO pin is being used as an INPUT
/* in      out    alt0   alt1   alt2   alt3   alt4   alt5 */
/* 0b000, 0b001, 0b100, 0b101, 0b110, 0b111, 0b011, 0b010*/
typedef enum {GPIO_INPUT     = 0b000,
              GPIO_OUTPUT    = 0b001,
              GPIO_ALT_FUNC0 = 0b100,
              GPIO_ALT_FUNC1 = 0b101,
              GPIO_ALT_FUNC2 = 0b110,
              GPIO_ALT_FUNC3 = 0b111,
              GPIO_ALT_FUNC4 = 0b011,
              GPIO_ALT_FUNC5 = 0b010,     /**< PWM */
          } FSEL;

/*
 * SetGPIOFunction function
 *  Parameters:
 *   pin   - number of GPIO pin;
 *   code  - alternate function code to which the GPIO pin is to be set
 *  Operation:
 *   Based on the specified GPIO pin number and function code, sets the GPIO pin to
 *   operate in the desired function. Each of the GPIO pins has at least two alternative functions.
 */
void SetGPIOFunction(int pin, FSEL code)
{
    int regIndex = pin / 10;
    int bit = (pin % 10) * 3;

    unsigned oldValue = gpio_regs->GPFSEL[regIndex];
    unsigned mask = 0b111 << bit;

    gpio_regs->GPFSEL[regIndex] = (oldValue & ~mask) | ((code << bit) & mask);
}

/*! Base Address of the PWM registers [bcm2835.h]*/
#define PWM_BASE (BCM2711_PERI_BASE + 0x20C000)
/*! Base Address of the Clock/timer registers [bcm2835.h]*/
#define PWM_CLK_BASE (BCM2711_PERI_BASE + 0x101000)
/* Defines for PWM Clock, word offsets (ie 4 byte multiples) [bcm2835.h - 998 and 999]*/
#define PWMCLK_CTL  40
#define PWMCLK_DIV  41

/* PWM registers */
/**< PWM ADRESS MAP [PAGE 141 -> BC2835 ARM PERIPHERALS][TABLE]*/
struct S_PWM_REGS
{
    uint32_t CTL;           /* PWM CONTROL*/
    uint32_t STA;           /* PWM STATUS*/
    uint32_t DMAC;          /* PWM DMA CONFIGURATION*/
    uint32_t reserved0;     
    uint32_t RNG1;          /* PWM CHANNEL 1 RANGE*/
    uint32_t DAT1;          /* PWM CHANNEL 1 DATA*/
    uint32_t FIF1;          /* PWM FIFO INPUT*/
    uint32_t reserved1;     
    uint32_t RNG2;          /* PWM CHANNEL 2 RANGE*/
    uint32_t DAT2;          /* PWM CHANNEL 2 DATA*/
} *pwm_regs;

/**< CTL REGISTER [PAGE 142 -> BC2835 ARM PERIPHERALS][TABLE]*/
/**< :1 <- mean 1 bit*/
volatile struct S_PWM_CTL {
    unsigned PWEN1 : 1;         /* CHANNEL 1 ENABLE*/
    unsigned MODE1 : 1;         /* CHANNEL 1 MODE*/
    unsigned RPTL1 : 1;         /* CHANEL 1 REPEAT LAST DATA*/
    unsigned SBIT1 : 1;         /* CHANEL 1 SILENCE BIT*/
    unsigned POLA1 : 1;         /* CHANNEL 1 POLARITY*/
    unsigned USEF1 : 1;         /* CHANNEL 1 USE FIFO*/
    unsigned CLRF1 : 1;         /* CLEAR FIFO*/
    unsigned MSEN1 : 1;         /* CHANNEL 1 M/S ENABLE*/
    unsigned PWEN2 : 1;         /* CHANNEL 2 ENABLE*/
    unsigned MODE2 : 1;         /* CHANNEL 2 MODE*/
    unsigned RPTL2 : 1;         /* CHANEL 2 REPEAT LAST DATA*/
    unsigned SBIT2 : 1;         /* CHANEL 2 SILENCE BIT*/
    unsigned POLA2 : 1;         /* CHANNEL 2 POLARITY*/
    unsigned USEF2 : 1;         /* CHANNEL 2 USE FIFO*/
    unsigned Reserved1 : 1;
    unsigned MSEN2 : 1;         /* CHANNEL 2 M/S ENABLE*/
    unsigned Reserved2 : 16;       
} *pwm_ctl;

/**< STA REGISTER [PAGE 144 -> BC2835 ARM PERIPHERALS][TABLE]*/
volatile struct S_PWM_STA {
    unsigned FULL1 : 1;         /* FIFO FULL FLAG*/
    unsigned EMPT1 : 1;         /* FIFO EMPTY FLAG*/
    unsigned WERR1 : 1;         /* FIFO WRITE ERROR FLAG*/
    unsigned RERR1 : 1;         /* FIFO READ ERROR FLAG*/
    unsigned GAPO1 : 1;         /* FIFO CHANNEL 1 GAP OCUURED FLAG*/
    unsigned GAPO2 : 1;         /* FIFO CHANNEL 2 GAP OCUURED FLAG*/
    unsigned GAPO3 : 1;         /* FIFO CHANNEL 3 GAP OCUURED FLAG*/
    unsigned GAPO4 : 1;         /* FIFO CHANNEL 4 GAP OCUURED FLAG*/
    unsigned BERR : 1;          /* BUS ERRPR FLAG*/
    unsigned STA1 : 1;          /* CHANNEL 1 STATE */
    unsigned STA2 : 1;          /* CHANNEL 2 STATE */
    unsigned STA3 : 1;          /* CHANNEL 3 STATE */
    unsigned STA4 : 1;          /* CHANNEL 4 STATE */
    unsigned Reserved : 19;
} *pwm_sta;

volatile unsigned int *pwm_clk_regs; //Holds the address of PWM CLK registers

/*
 * Establish PWM frequency function
 * Parameters:
 * divi   - integer part of divisor.
 * Operation:
 * Based on the passed GPIO pin number and function code, sets the GPIO pin to
 * operate the desired function. Each of the GPIO pins has at least two alternative functions.
 */
void pwm_frequency(uint32_t divi) {

    // Kill the clock
    *(pwm_clk_regs+PWMCLK_CTL) = 0x5A000020;

    // Disable PWM
    pwm_ctl->PWEN1 = 0;
    pwm_ctl->PWEN2 = 0;
    udelay(10);

    // Set the divisor
    *(pwm_clk_regs+PWMCLK_DIV) = 0x5A000000 | (divi << 12);

    // Set source to oscillator and enable clock
    *(pwm_clk_regs+PWMCLK_CTL) = 0x5A000011;
}

void set_up_pwm_channels(void){
    // Channel 1 set-up
    pwm_ctl->MODE1 = 0;
    pwm_ctl->RPTL1 = 0;
    pwm_ctl->SBIT1 = 0;
    pwm_ctl->POLA1 = 0;
    pwm_ctl->USEF1 = 0;
    pwm_ctl->CLRF1 = 1;
    pwm_ctl->MSEN1 = 1;

    // Channel 2 set-up
    pwm_ctl->MODE2 = 0;
    pwm_ctl->RPTL2 = 0;
    pwm_ctl->SBIT2 = 0;
    pwm_ctl->POLA2 = 0;
    pwm_ctl->USEF2 = 0;
    pwm_ctl->MSEN2 = 1;
}

void pwm_ratio_c1(unsigned n, unsigned m) {

    // Disable PWM Channel 1
    pwm_ctl->PWEN1 = 0;

    // Set the PWM Channel 1 Range Register
    pwm_regs->RNG1 = m;
    // Set the PWM Channel 1 Data Register
    pwm_regs->DAT1 = n;

    // Check if PWM Channel 1 is not currently transmitting
    if ( !pwm_sta->STA1 ) {
        if ( pwm_sta->RERR1 ) pwm_sta->RERR1 = 1; // Clear RERR bit if read occured on empty FIFO while channel was transmitting
        if ( pwm_sta->WERR1 ) pwm_sta->WERR1 = 1; // Clear WERR bit if write occured on full FIFO while channel was transmitting
        if ( pwm_sta->BERR ) pwm_sta->BERR = 1; // Clear BERR bit if write to registers via APB occured while channel was transmitting
    }
    udelay(10);

    // Enable PWM Channel 1
    pwm_ctl->PWEN1 = 1;
}

void pwm_ratio_c2(unsigned n, unsigned m) {

    // Disable PWM Channel 2
    pwm_ctl->PWEN2 = 0;

    // Set the PWM Channel 2 Range Register
    pwm_regs->RNG2 = m;
    // Set the PWM Channel 2 Data Register
    pwm_regs->DAT2 = n;

    // Check if PWM Channel 2 is not currently transmitting
    if ( !pwm_sta->STA2 ) {
        if ( pwm_sta->RERR1 ) pwm_sta->RERR1 = 1; // Clear RERR bit if read occured on empty FIFO while channel was transmitting
        if ( pwm_sta->WERR1 ) pwm_sta->WERR1 = 1; // Clear WERR bit if write occured on full FIFO while channel was transmitting
        if ( pwm_sta->BERR ) pwm_sta->BERR = 1; // Clear BERR bit if write to registers via APB occured while channel was transmitting
    }
    udelay(10);

    // Enable PWM Channel 2
    pwm_ctl->PWEN2 = 1;
}


/* Declaration of my_pwm_driver.c functions */
int my_pwm_driver_init(void);
void my_pwm_driver_exit(void);
static int my_pwm_driver_open(struct inode *, struct file *);
static int my_pwm_driver_release(struct inode *, struct file *);
static ssize_t my_pwm_driver_read(struct file *, char *buf, size_t , loff_t *);
static ssize_t my_pwm_driver_write(struct file *, const char *buf, size_t , loff_t *);

/* Structure that declares my_pwm_driver file operations */
struct file_operations my_pwm_driver_fops =
{
    open            :   my_pwm_driver_open,
    release         :   my_pwm_driver_release,
    read            :   my_pwm_driver_read,
    write           :   my_pwm_driver_write
};

/* Declaration of the init and exit functions. */
module_init(my_pwm_driver_init);
module_exit(my_pwm_driver_exit);

/* Major number. */
/*int my_pwm_driver_major;
*/

/* Device variables */
struct class* buzzDevice_class = NULL; //class pointer
//static struct device* buzzDevice_device = NULL;
static dev_t buzzDevice_majorminor; //device
static struct cdev c_dev;  // Character device structure

/**< Ioremap() -> returns a kernel virtual address corresponding to start of the requested physical address range */
int my_pwm_driver_init(void)
{
    int result = -1;
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "Inserting my_pwm_driver module..\n");

        /**< Allocatesa range of char device numbers. The major number will be chosen dynamically, and returned */
    if ((ret = alloc_chrdev_region(&buzzDevice_majorminor, 0, 1, "my_pwm_driver")) < 0) {
        return ret;
    }

    /**< IS_ERR is used to check if class_create succeded creating ledDevice_class. If an error occurs unregister 
     * the char driver and signal the error. */
    if (IS_ERR(buzzDevice_class = class_create(THIS_MODULE, "my_pwm_driverClass"))) {
        unregister_chrdev_region(buzzDevice_majorminor, 1);
        return PTR_ERR(buzzDevice_class);
    }

    /**< Creates a device and registers it.*/
    if (IS_ERR(dev_ret = device_create(buzzDevice_class, NULL, buzzDevice_majorminor, NULL, "my_pwm_driver"))) {
        class_destroy(buzzDevice_class);
        unregister_chrdev_region(buzzDevice_majorminor, 1);
        return PTR_ERR(dev_ret);
    }

    /**< Creates (initializes) a character device */
    cdev_init(&c_dev, &my_pwm_driver_fops);
    c_dev.owner = THIS_MODULE;
    /**< Add a char device to the system */
    if ((ret = cdev_add(&c_dev, buzzDevice_majorminor, 1)) < 0) {
        printk(KERN_NOTICE "Error %d adding device", ret);
        device_destroy(buzzDevice_class, buzzDevice_majorminor);
        class_destroy(buzzDevice_class);
        unregister_chrdev_region(buzzDevice_majorminor, 1);
        return ret;
    }

    // Map the GPIO register space from PHYSICAL address space to VIRTUAL address space
    gpio_regs = (struct S_GPIO_REGS *)ioremap(GPIO_BASE, sizeof(struct S_GPIO_REGS));
    if(!gpio_regs)
    {
        result = -ENOMEM;
        goto fail_no_virt_mem;
    }

    // Map the PWM register space from PHYSICAL address space to VIRTUAL address space
    pwm_regs = (struct S_PWM_REGS *)ioremap(PWM_BASE, sizeof(struct S_PWM_REGS));
    if(!pwm_regs)
    {
        result = -ENOMEM;
        goto fail_no_virt_mem;
    }
    pwm_ctl = (struct S_PWM_CTL *) &pwm_regs -> CTL;
    pwm_sta = (struct S_PWM_STA *) &pwm_regs -> STA;

    // Map the PWM Clock register space from PHYSICAL address space to VIRTUAL address space
    pwm_clk_regs = ioremap(PWM_CLK_BASE, 4096);
    if(!pwm_clk_regs)
    {
        result = -ENOMEM;
        goto fail_no_virt_mem;
    }

    // Setting the GPIO pins alternative functions to PWM
    SetGPIOFunction(GPIO_18, GPIO_ALT_FUNC5); // Setting GPIO_18 pin to alternative function 5 - PWM channel 0
    SetGPIOFunction(GPIO_19, GPIO_ALT_FUNC5); // Setting GPIO_19 pin to alternative function 5 - PWM channel 1

    // Set up PWM channels
    set_up_pwm_channels();

    pwm_frequency(CLK_DIVISOR); 
    pwm_ratio_c1(UP, RANGE);
    pwm_ratio_c2(UP, RANGE);

    return 0;

fail_no_virt_mem:

    // Unmap the GPIO registers PHYSICAL address space from VIRTUAL memory
    if (gpio_regs)
        iounmap(gpio_regs);
    // Unmap the PWM registers PHYSICAL address space from VIRTUAL memory
    if (pwm_regs)
        iounmap(pwm_regs);
    // Unmap the PWM Clock registers PHYSICAL address space from VIRTUAL memory
    if (pwm_clk_regs)
        iounmap(pwm_clk_regs);

    /* Freeing the major number. */
    //unregister_chrdev(my_pwm_driver_major, "my_pwm_driver");
    cdev_del(&c_dev);
    device_destroy(buzzDevice_class, buzzDevice_majorminor);
    class_destroy(buzzDevice_class);
    unregister_chrdev_region(buzzDevice_majorminor, 1);

    return result;
}

void my_pwm_driver_exit(void)
{
    // Clearing the GPIO pins - setting to LOW
    gpio_regs->GPCLR[GPIO_18/32] |= (1 << (GPIO_18 % 32));
    gpio_regs->GPCLR[GPIO_19/32] |= (1 << (GPIO_19 % 32));

    // Setting the GPIO pins to default function - as INPUT
    SetGPIOFunction(GPIO_18, GPIO_INPUT);
    SetGPIOFunction(GPIO_19, GPIO_INPUT);

    // Unmap the GPIO registers PHYSICAL address space from VIRTUAL memory
    if (gpio_regs)
        iounmap(gpio_regs);
    // Unmap the PWM registers PHYSICAL address space from VIRTUAL memory
    if (pwm_regs)
        iounmap(pwm_regs);
    // Unmap the PWM Clock registers PHYSICAL address space from VIRTUAL memory
    if (pwm_clk_regs)
        iounmap(pwm_clk_regs);

    printk(KERN_INFO "Removing my_pwm_driver module\n");

    /* Freeing the major number. */
    //unregister_chrdev(my_pwm_driver_major, "my_pwm_driver");
    cdev_del(&c_dev);
    device_destroy(buzzDevice_class, buzzDevice_majorminor);
    class_destroy(buzzDevice_class);
    unregister_chrdev_region(buzzDevice_majorminor, 1);
}

static int my_pwm_driver_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int my_pwm_driver_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t my_pwm_driver_read(struct file *filp, char *buf, size_t len, loff_t *f_pos)
{
    printk(KERN_ALERT "Sorry, this operation my_pwm_driver_read isn't supported.\n");

    return 0; // Operation not permitted error
}


const char comand[6][4] = 
{
    "1u","1m","1d",
    "2u","2m","2d"
};

void (*func_ptr[6])(unsigned, unsigned) = {pwm_ratio_c1, pwm_ratio_c1, pwm_ratio_c1, pwm_ratio_c2, pwm_ratio_c2, pwm_ratio_c2};

const unsigned dutty[6] = {UP,MIDDLE,DOWN,UP,MIDDLE,DOWN};

static ssize_t my_pwm_driver_write(struct file *filp, const char *buf, size_t len, loff_t *f_pos)
{
    int i;
    char aux[16];

    for(i = 0; i < len-1; i++)
        aux[i]=buf[i];
    aux[i] = '\0';

    printk(KERN_ALERT "comand %s\n",aux);

    for(i = 0; i < 6; i++)
        if(!strcmp(comand[i], aux))
            (*func_ptr[i])(dutty[i],RANGE);

    return len;
}