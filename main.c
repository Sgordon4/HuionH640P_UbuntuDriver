#include "device_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h> /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */

MODULE_LICENCE("IDK, haven't looked into this lol");
MODULE_AUTHOR("Sean Gordon");


static int huion_driver_init(void){
    int result = 0;
    printk( KERN_NOTICE "Huion-driver: Initialization started" );
    
    result = register_device();
    return result;
}

static void huion_driver_exit(void){
    printk( KERN_NOTICE "Huion-driver: Exiting" );
    unregister_device();
}


module_init(huion_driver_init);
module_exit(huion_driver_exit);
