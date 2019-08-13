#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>

//#include <linux/irq.h>
#include <linux/interrupt.h>

//#include <asm/irq.h>
#include <asm/io.h>


#define DEVICE_NAME "DrawpadDriver"
#define DRAWPAD_IRQ 13

MODULE_AUTHOR("Sean Gordon <SeanGordonkh@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple input device driver designed for the "
                   "Huion H640P drawing tablet");


static struct input_dev *drawpad_dev;


static irqreturn_t drawpad_interrupt(int irq, void *dummy)
{
    printk(KERN_ALERT "Interrupt called!");
    return IRQ_HANDLED;
}


static int __init drawpad_init(void)
{
    int error;

    printk(KERN_INFO "--------------------");
    printk(KERN_INFO "input_driver loaded!");
    printk(KERN_INFO "--------------------");

    if (request_irq(DRAWPAD_IRQ, drawpad_interrupt, 0, DEVICE_NAME, drawpad_dev)) {
            printk(KERN_ERR "button.c: Can't allocate irq %d\n", DRAWPAD_IRQ);
            return -EBUSY;
    }


    drawpad_dev = input_allocate_device();
    if (!drawpad_dev) {
            printk(KERN_ERR "input_driver.c: Not enough memory\n");
            error = -ENOMEM;
            goto err_free_irq;
    }


    //Fill in some device information
    drawpad_dev->name = DEVICE_NAME;
    drawpad_dev->uniq = 0x256c;
    drawpad_dev->id   = 0x006d;
    




    error = input_register_device(drawpad_dev);
    if (error) {
            printk(KERN_ERR "input_driver.c: Failed to register device\n");
            goto err_free_dev;
    }

    return 0;

    // Ensure if errors are thrown, all appropriate resources are freed
    err_free_dev:
            input_free_device(drawpad_dev);
    err_free_irq:
            free_irq(DRAWPAD_IRQ, drawpad_interrupt);
            return error;
}

static void __exit drawpad_exit(void)
{

    input_unregister_device(drawpad_dev);
    free_irq(DRAWPAD_IRQ, drawpad_dev);

    printk(KERN_INFO "----------------------");
    printk(KERN_INFO "input_driver unloaded!");
    printk(KERN_INFO "----------------------");
}


module_init(drawpad_init);
module_exit(drawpad_exit);
