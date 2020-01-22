

//To register a device with a driver, register the device with the
//Kernel with a major device number using register_chrdev, and
//register the driver with the same major number


//Work referencing
//https://elixir.bootlin.com/linux/latest/source/drivers/hid/usbhid/usbmouse.c




static int usb_drawpad_open(struct input_dev *dev)
{
	struct usb_drawpad *drawpad = input_get_drvdata(dev);

	drawpad->irq->dev = drawpad->usbdev;
	if (usb_submit_urb(drawpad->irq, GFP_KERNEL))
		return -EIO;

	return 0;
}

static void usb_drawpad_close(struct input_dev *dev)
{
	struct usb_drawpad *drawpad = input_get_drvdata(dev);

	usb_kill_urb(drawpad->irq);
}



/* Called when a device listed in this driver's registered ID table
 * is plugged in.
 * Taking the passed interface, ensure there is an interrupt endpoint,
 * and that it is an incoming line.
 * Then, allocate the necessary size of this structure in the kernel,
 * and assign all irq and urb related information necessary.
 * Compile the name for the device, and grab the physical path
 * to the device.
 * Finally, set what each bit does and link functions.
 */
static int usb_drawpad_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    printk(KERN_ALERT "--------------------------------");
    printk(KERN_ALERT "Drawpad probe called");
    printk(KERN_ALERT "Device (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    printk(KERN_ALERT "--------------------------------");


    struct usb_device *usb_dev = interface_to_usbdev(intf);

	struct usb_host_interface *host_interface;
	struct usb_endpoint_descriptor *endpoint;

	struct usb_drawpad *drawpad;
	struct input_dev *input_dev;

	int pipe, maxp;
	int error = -ENOMEM;



    //Grab the current settings from the passed interface
    host_interface = intf->cur_altsetting;

    //If there is no interrupt endpoint...
    if(host_interface->desc.bNumEndpoints != 1)
        return -ENODEV;     //There is no device we care about

    //Grab the endpoint
    endpoint = &host_interface->endpoint[0].desc;
    //If the endpoint is not an incoming line
    if(!usb_endpoint_is_int_in(endpoint))
        return -ENODEV;     //There is no device we care about


    //Grab the IN endpoint for the device. Transfer buffer
    pipe = usb_rcvintpipe(usb_dev, endpoint->bEndpointAddress);
    //A ten minute search yielded no results. I have no literal
    //definition for this function. I can only assume it represents
    //the maximum packet size the device supports.
    //Length of the transfer buffer
    maxp = usb_maxpacket(usb_dev, pipe, usb_pipeout(pipe));



    /*----------------------- Allocation ----------------------*/
    //Allocate enough space for the drawpad struct in kernel memory
    drawpad = kzalloc(sizeof(struct usb_drawpad), GFP_KERNEL);
    input_dev = input_allocate_device();

    //If either allocation failed...
    if(!drawpad || !input_dev)
        goto failALLOC;


    //Allocate a DMA-consistent buffer
    drawpad->data = usb_alloc_coherent(usb_dev, 8, GFP_ATOMIC, &drawpad->data_dma);
    if(!drawpad->data)
        goto failDMA;


    //Create a new urb for the driver to use
    drawpad->irq = usb_alloc_urb(0, GFP_KERNEL);
    if(!drawpad->irq)
        goto failIRQ;


    //Get the physial path for the drawpad
    usb_make_path(usb_dev, drawpad->phys, sizeof(drawpad->phys));
    strlcat(drawpad->phys, "/input0", sizeof(drawpad->phys));

    //Add new info to struct
    drawpad->usbdev = usb_dev;
    drawpad->dev    = input_dev;



    /*-------------------------- Name --------------------------*/
    //Create a name for the device
    if (usb_dev->manufacturer)
		strlcpy(drawpad->name, usb_dev->manufacturer, sizeof(drawpad->name));

	if (usb_dev->product) {
		if (usb_dev->manufacturer)
			strlcat(drawpad->name, " ", sizeof(drawpad->name));
		strlcat(drawpad->name, usb_dev->product, sizeof(drawpad->name));
	}

    if (!strlen(drawpad->name))
		snprintf(drawpad->name, sizeof(drawpad->name),
			"USB HIDBP Drawpad %04x:%04x",
			le16_to_cpu(usb_dev->descriptor.idVendor),
			le16_to_cpu(usb_dev->descriptor.idProduct));




    /*----------------------- Final Info -----------------------*/

    //Copy the information from our drawpad struct to our temp input_dev struct
    input_dev->name = drawpad->name;
    input_dev->phys = drawpad->phys;

    //Get the id for the usb device
    usb_to_input_id(usb_dev, &input_dev->id);

    //Assign the passed interface as the input_dev parent
    input_dev->dev.parent = &intf->dev;


    input_set_drvdata(input_dev, drawpad);

    input_dev->open = usb_drawpad_open;
    input_dev->close = usb_drawpad_close;


    /*----------------------- Bit Assign -----------------------*/





    /*---------------------- Registration ----------------------*/




    //Initialize an interrupt urb
    usb_fill_int_urb(drawpad->irq, usb_dev, pipe, drawpad->data,
                     (maxp > 8 ? 8 : maxp), usb_drawpad_irq,
                     drawpad, endpoint->bInterval
                    );



    drawpad->irq->transfer_dma = drawpad->data_dma;
    drawpad->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;


    //Finally register the device
    error = input_register_device(drawpad->dev);
    if(error)
        goto failRegister;

    //Save a pointer to the driver specific structure 'drawpad'
    //Can be retrieved later using usb_get_intfdata()
    usb_set_intfdata(intf, drawpad);

    return 0;



failRegister:
    usb_free_urb(drawpad->irq);
failIRQ:
    usb_free_coherent(usb_dev, 8, drawpad->data, drawpad->data_dma);
failDMA:
failALLOC:
    input_free_device(input_dev);
    kfree(drawpad);

    printk(KERN_ERROR "There was an error in drawpad_driver probe!")

    return error;
}







//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------






// #include <linux/init.h>
#include <linux/module.h>
// #include <linux/kernel.h>
// #include <linux/input.h>
// #include <linux/usb.h>
// #include <linux/interrupt.h>
// #include <libusb-1.0/libusb.h>

#include <linux/kernel.h>
#include <linux/slab.h>
//#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>




// Struct representing the drawpad this driver runs
struct usb_drawpad{
    char name[128];             // Name of the usb drawpad
    char phys[64];              // Physical path to the usb drawpad
    struct usb_device *usbdev;  //
    struct input_dev *dev;      // Holds all the information about the drawpad
    struct urb *irq;            //

    __u8 bulk_in_endpointAddr;
    __u8 bulk_out_endpointAddr;

    signed char *data;
    dma_addr_t data_dma;        // Holds the Dynamic Memory Address (DMA)
};





static void usb_drawpad_irq(struct urb *urb)
{
    printk(KERN_ALERT "--------------------------------");
    printk(KERN_ALERT "Interrupt Handler Called");
    printk(KERN_ALERT "--------------------------------");
}



static int usb_drawpad_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    printk(KERN_ALERT "--------------------------------");
    printk(KERN_ALERT "USB Drawpad Probe Called!");
    printk(KERN_ALERT "--------------------------------");


    struct usb_drawpad *dev;
    struct input_dev *input_dev;

    struct usb_interface *interface;

    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;



    //Grab the current settings from the passed interface
    iface_desc = intf->cur_altsetting;

    int i;
    //For every enpoint in the interface
    for(i = 0; i < iface_desc->desc.bNumEndpoints; ++i){

        //Add a local pointer to it to the endpoint structure for easier access
        endpoint = &iface_desc -> endpoint[i].desc;

        //If we have not yet found a 'bulk-in' endpoint...
        if(!dev->bulk_in_endpointAddr &&
            //Check if the one we're now looking at is 'in'
            (!usb_endpoint_is_int_in(endpoint)) &&
            //Then check if it is bulk
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))
        {
            //We have found what we're looking for:
            //(an incoming endpoint for bulk style transfer)

            //Find the endpoint's maximum packet size and allocate enough space for a buffer
            int buffer_size = endpoint -> wMaxPacketSize;
            dev->bulk_in_size = buffer_size;
            dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);

            //If the buffer was not allocated...
            if(!dev->bulk_in_buffer){
                printk(KERN_ERR "Could not allocate bulk_in_buffer");
                goto error;
            }
        }
        /*

        //If we have not yet found a 'bulk-out' endpoint...
        if (!dev->bulk_out_endpointAddr &&
            //Check if the one we are now looking at is 'out'
            !(endpoint->bEndpointAddress & USB_DIR_OUT) &&
            //Then check if it is bulk
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))
        {
            //We found a bulk out endpoint
            dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
        }
*/

        if (!(dev->bulk_in_endpointAddr /*&& dev->bulk_out_endpointAddr*/)) {
            printk(KERN_ERR "Could not find both bulk-in and bulk-out endpoints");
            goto error;
        }

    }


    //Save our data pointer in this interface device
    usb_set_intfdata(interface, dev);

    return 0;



    error:
    kfree(buffer_size);
}





static void usb_drawpad_disconnect(struct usb_interface *intf)
{
    printk(KERN_ALERT "--------------------------------");
    printk(KERN_ALERT "USB Drawpad Disconnected!");
    printk(KERN_ALERT "--------------------------------");

    struct usb_drawpad *dev;

    int minor = interface->minor;

    /* prevent skel_open( ) from racing skel_disconnect( ) */
    lock_kernel( );

    dev = usb_get_intfdata(interface);
    usb_set_intfdata(interface, NULL);

    /* give back our minor */
    usb_deregister_dev(interface, &drawpad_class);

    unlock_kernel( );

    /* decrement our usage count */
    kref_put(&dev->kref, usb_drawpad_delete);
    printk(KERN_ALERT "USB drawpad #%d now disconnected", minor);
}




//Initial connection:
//idVendor=0416, idProduct=3f00

static struct usb_device_id usb_drawpad_id_table[] = {
    {USB_DEVICE(0x0416, 0x3f00)},   //Initial device connection
    {USB_DEVICE(0x256c, 0x006d)},   //Huion H640P drawpad
	{ }	/* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, usb_drawpad_id_table);


static struct usb_driver usb_drawpad_driver = {
	.name		= "usbdrawpad",
	.probe		= usb_drawpad_probe,
	.disconnect	= usb_drawpad_disconnect,
	.id_table	= usb_drawpad_id_table,
};
