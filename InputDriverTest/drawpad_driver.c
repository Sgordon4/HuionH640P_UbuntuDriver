#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/slab.h>

#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
// #include <linunx/ioport.h>

#include <linux/io.h>



MODULE_AUTHOR("Sean Gordon <SeanGordonkh@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple input device driver designed for the "
                   "Huion H640P drawing tablet");





// Struct representing the drawpad this driver runs
struct usb_drawpad{
    char name[128];             // Name of the usb drawpad
    char phys[64];              // Physical path to the usb drawpad
    struct usb_device *usbdev;  //
    struct input_dev *dev;      // Holds all the information about the drawpad
    struct urb *irq;            //

    signed char *data;
    dma_addr_t data_dma;        // Holds the Dynamic Memory Address (DMA)
};




//TODO
static ssize_t read_drawpad(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
    return -EINVAL;
}

//Can't really write to a drawpad
static ssize_t write_drawpad(struct file *filp, const char *buffer, size_t count, loff_t *ppos)
{
    return -EINVAL;
}



static int drawpad_users = 0;       //# of current users
static int drawpad_dx = 0;          //X axis position
static int drawpad_dy = 0;          //Y axis position
static int drawpad_event = 0;       //'Something has happened'

static struct wait_queue_head *drawpad_wait;
//static spinlock_t drawpad_lock;
//DEFINE_SPINLOCK(drawpad_lock);


//TODO
static unsigned int poll_drawpad(struct file *filp, poll_table *wait)
{
    poll_wait(filp, drawpad_wait, wait);

    if(drawpad_event)
        return POLLIN | POLLRDNORM;
    return 0;
}

//TODO
/*
static irq_handler drawpad_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    printk(KERN_ALERT "Interrupted!");
    return 0;
}
*/
static irqreturn_t drawpad_interrupt(int irq, void *dummy)
{
    printk(KERN_ALERT "Interrupt called!\n");
    return IRQ_HANDLED;
}



//Find this programmatically later
int DRAWPAD_IRQ = 8;

static int open_drawpad(struct inode *inode, struct file *file)
{
    //Does two things:
    //Checks if drawpad_users is not initially 0 (first user)
    //Increments the active user count
    if(drawpad_users++)
        return 0;

    //If this is the first user...
    if(request_irq(DRAWPAD_IRQ, drawpad_interrupt, 0, "Huion drawpad", NULL))
    {
        //If the request failed
        drawpad_users--;
        return -EBUSY;
    }

    drawpad_dx = 0;
    drawpad_dy = 0;
    drawpad_event = 0;
    //drawpad_buttons = 0;

    return 0;
}


static int close_drawpad(struct inode *inode, struct file *file)
{
    //Does two things:
    //Decrements the active user count
    //Checks if that wasn't the last user
    if(--drawpad_users)
        return 0;

    //If that was the last user...
    free_irq(DRAWPAD_IRQ, NULL);

    return 0;
}





struct file_operations drawpad_fops = {
    .owner      = THIS_MODULE,
    //.seek     =
    .read       = read_drawpad,
    .write      = write_drawpad,
    //.readdir  =
    .poll       = poll_drawpad,
    //.ioctl    =
    //mmap      =
    .open       = open_drawpad,
    //.flush    =
    .release    = close_drawpad,
    //.fsync    =
    //.fasync   =
    //.lock     =
};


#define DRAWPAD_START   0x81
#define DRAWPAD_END     0x82

static struct miscdevice drawpad_misc = {
    .minor      = MISC_DYNAMIC_MINOR,
    .name       = "drawpad",
    .fops       = &drawpad_fops,
};





static int __init drawpad_init(void)
{
    printk(KERN_ALERT "\nInitializing drawpad driver!\n");


    //Claim our drawpad's IO ports
    if(request_region(DRAWPAD_START, 2, "drawpad") == NULL) {
        printk(KERN_ERR "Drawpad request_region failed.\n");
        return -ENODEV;
    }

    //Register this driver
    if (misc_register(&drawpad_misc) < 0){
        printk(KERN_ERR "Cannot register device!\n");
        release_region(DRAWPAD_START, 2);
        return -EBUSY;
    }

    return 0;
}
static void __exit drawpad_exit(void)
{
    misc_deregister(&drawpad_misc);
    release_region(DRAWPAD_START, 2);
}
module_init(drawpad_init);
module_exit(drawpad_exit);










/*
 * Module init/exit can be replaced with the below module_usb_driver() macro,
 * as there's nothing really special going on in them.
 */
// static int __init drawpad_init(void)
// {
//     int result;
//
//     //Register this driver with the usb subsystem
//     result = usb_register(&usb_drawpad_driver);
//     if(result)
//         printk(KERN_ERR "USB register failed with error number %d", result);
//
//     return result;
// }
// static void __exit drawpad_exit(void)
// {
//     //Deregister this driver with the usb subsystem
//     usb_deregister(&usb_drawpad_driver);
// }
//
// module_init(drawpad_init);
// module_exit(drawpad_exit);




// module_usb_driver() - Helper macro for registering a USB driver
//module_usb_driver(drawpad_fops);
