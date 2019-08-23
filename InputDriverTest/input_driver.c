#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/usb.h>

#include <libusb-1.0/libusb.h>

//#include <linux/irq.h>
#include <linux/interrupt.h>

//#include <asm/irq.h>
#include <asm/io.h>


//Defined in pci_ids.h
#define PCI_CLASS_INPUT_PEN		0x0901

#define DEVICE_NAME "DrawpadDriver"
#define DRAWPAD_IRQ 13

MODULE_AUTHOR("Sean Gordon <SeanGordonkh@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple input device driver designed for the "
                   "Huion H640P drawing tablet");


static struct input_dev *drawpad_dev;

static struct input_id huion_drawpad = {
    //.bustype    =
    .vendor     = 0x256c,
    .product    = 0x006d,
    //.version    =

};


// Decide if the passed device is one we want to control with this driver.
// @param device Device to check for interesting qualities.
// @return interesting Returns 1 if the device is interesting, 0 otherwise.
private int is_interesting(libusb_device *device){
    int interesting = 0;
    printk(KERN_ALERT "is_interesting called!");

    return interesting;
}

// Find a device that we want to control with this driver.
// @param context Context of current libusb instance (can be NULL).
// @param found Interesting device that we are searching for.
// @param handle Device handler for the interesting device we are searching for.
// @return err Returns 0 for success, error code otherwise
private int find_interesting_device(libusb_context *context, libusb_device *found, libusb_device_handle *handle){
    found = NULL;
    libusb_device **list;


    ssize_t count = libusb_get_device_list(context, &list);
    ssize_t i = 0;
    int err = 0;

    //If no devices are found...
    if (count < 0)
        error();

    //For every device found...
    for (i = 0; i < count; i++) {
        libusb_device *device = list[i];
        if (is_interesting(device)) {
            found = device;
            break;
        }
    }
    if (found) {
        libusb_device_handle *handle;
        err = libusb_open(found, &handle);
        if (err)
            error();
        // etc
    }
    libusb_free_device_list(list, 1);

    //Will return 0 upon success
    return err;
}

// Table of devices that work with this driver
// Enables the linux-hotplug system to load driver automatically upon device plug-in
static struct usb_device_id drawpad_table [] =
{
    { USB_DEVICE(0x256c, 0x006d) }, //Huion H640P Drawing Tablet
    { } //Terminating entry
};
MODULE_DEVICE_TABLE (usb, drawpad_table);


//Called when a device registered in the above 'drawpad_table' table is plugged in
static int drawpad_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Device (%04X:%04X) plugged\n",
                        id->idVendor, id->idProduct);
    return 0;
}

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
    // drawpad_dev->uniq = 0x256c;
    drawpad_dev->id   = huion_drawpad;





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


static
