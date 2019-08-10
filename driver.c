#include "driver.h"

#include <linux/init.h>   // module_init, module_exit
#include <linux/module.h> // version info, MODULE_LICENSE, MODULE_AUTHOR, printk()
#include <linux/kernel.h>
#include <linux/usb.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sean Gordon");
MODULE_DESCRIPTION("USB Drawing Pad Driver");






// Table of devices that work with this driver
// Enables the linux-hotplug system to load driver automatically upon device plug-in
static struct usb_device_id drawpad_table [] =
{
    { USB_DEVICE(0x256c, 0x006d) }, //Huion H640P Drawing Tablet
    { } //Terminating entry
};
MODULE_DEVICE_TABLE (usb, drawpad_table);


static struct usb_driver drawpad_driver = {
        .name        = "HuionDriver",
        .probe       = drawpad_probe,
        .disconnect  = drawpad_disconnect,
        // .unlocked_ioctl =
        // .suspend     =
        // .resume      =
        // .reset_resume=
        // .pre_reset   =
        // .post_reset  =
        .id_table    = drawpad_table,
        // .fops        = &drawpad_fops,
        // .minor       = USB_DRAWPAD_MINOR_BASE,
};



//Called when a device registered in the above 'drawpad_table' table is plugged in
static int drawpad_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Device (%04X:%04X) plugged\n",
                        id->idVendor, id->idProduct);
    return 0;
}

//Called when device is disconnected or driver module is being unloaded
static void drawpad_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Device removed\n");
}






static int __init usb_drawpad_init(void)
{
        int result;
        printk( KERN_NOTICE "Huion-driver: Initialization started" );

        //Register this driver with the USB subsystem
        result = usb_register(&drawpad_driver);

        //If registration failed
        if (result < 0) {
            printk( KERN_NOTICE "USB_register failed for the driver.\nError number %d", result);
                return -1;
        }

        return 0;
}


static void __exit usb_drawpad_exit(void)
{
        printk( KERN_NOTICE "Huion-driver: Exiting" );
        /* deregister this driver with the USB subsystem */
        usb_deregister(&drawpad_driver);
}


module_init(usb_drawpad_init);
module_exit(usb_drawpad_exit);
