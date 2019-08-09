#include "device_file.h"
#include <linux/init.h>   // module_init, module_exit
#include <linux/module.h> // version info, MODULE_LICENSE, MODULE_AUTHOR, printk()

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sean Gordon");



/* table of devices that work with this driver */
// Enables the linux-hotplug system to load driver automatically upon device plug-in
static struct usb_device_id skel_table [] = {
        { USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },
        { }                      /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, skel_table);


static struct usb_driver skel_driver = {
        .name        = "skeleton",
        .probe       = skel_probe,
        .disconnect  = skel_disconnect,
        .fops        = &skel_fops,
        .minor       = USB_SKEL_MINOR_BASE,
        .id_table    = skel_table,
};



static int __init usb_skel_init(void)
{
        int result;
        printk( KERN_NOTICE "Huion-driver: Initialization started" );

        /* register this driver with the USB subsystem */
        result = usb_register(&skel_driver);
        if (result < 0) {
                err("usb_register failed for the "__FILE__ "driver."
                    "Error number %d", result);
                return -1;
        }

        return 0;
}


static void __exit usb_skel_exit(void)
{
        printk( KERN_NOTICE "Huion-driver: Exiting" );
        /* deregister this driver with the USB subsystem */
        usb_deregister(&skel_driver);
}


module_init(usb_skel_init);
module_exit(usb_skel_exit);

