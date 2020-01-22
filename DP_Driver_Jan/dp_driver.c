#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Sean Gordon <SeanGordonkh@gmail.com>"
#define DRIVER_DESC "USB Drawpad driver"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

struct usb_drawpad {
	char name[128];
	char phys[64];
	struct usb_device *usbdev;
	struct input_dev *dev;
	struct urb *irq;

	signed char *data;
	dma_addr_t data_dma;
};

static void usb_drawpad_irq(struct urb *urb)
{
    printk(KERN_INFO "----------------------\n");
	printk(KERN_INFO "We here \n");
	printk(KERN_INFO "----------------------\n");
}


static int usb_drawpad_open(struct input_dev *dev)
{
    printk(KERN_INFO "----------------------\n");
    printk(KERN_INFO "input_driver opened!\n");
    printk(KERN_INFO "----------------------\n");

	struct usb_drawpad *drawpad = input_get_drvdata(dev);

	drawpad->irq->dev = drawpad->usbdev;
	if (usb_submit_urb(drawpad->irq, GFP_KERNEL))
		return -EIO;

	return 0;
}

static void usb_drawpad_close(struct input_dev *dev)
{
    printk(KERN_INFO "----------------------\n");
    printk(KERN_INFO "input_driver closed!\n");
    printk(KERN_INFO "----------------------\n");
	struct usb_drawpad *drawpad = input_get_drvdata(dev);

	usb_kill_urb(drawpad->irq);
}


static int usb_drawpad_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    printk(KERN_INFO "----------------------\n");
    printk(KERN_INFO "input_driver probed!\n");
    printk(KERN_INFO "----------------------\n");

	//return 0;
    /*
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_drawpad *drawpad;
	struct input_dev *input_dev;
	int pipe, maxp;
	int error = -ENOMEM;

	interface = intf->cur_altsetting;

	if (interface->desc.bNumEndpoints != 1)
		return -ENODEV;

	endpoint = &interface->endpoint[0].desc;
	if (!usb_endpoint_is_int_in(endpoint))
		return -ENODEV;

	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));

	drawpad = kzalloc(sizeof(struct usb_drawpad), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!drawpad || !input_dev)
		goto fail1;

	drawpad->data = usb_alloc_coherent(dev, 8, GFP_ATOMIC, &drawpad->data_dma);
	if (!drawpad->data)
		goto fail1;

	drawpad->irq = usb_alloc_urb(0, GFP_KERNEL);
	if (!drawpad->irq)
		goto fail2;

	drawpad->usbdev = dev;
	drawpad->dev = input_dev;

	if (dev->manufacturer)
		strlcpy(drawpad->name, dev->manufacturer, sizeof(drawpad->name));

	if (dev->product) {
		if (dev->manufacturer)
			strlcat(drawpad->name, " ", sizeof(drawpad->name));
		strlcat(drawpad->name, dev->product, sizeof(drawpad->name));
	}

	if (!strlen(drawpad->name))
		snprintf(drawpad->name, sizeof(drawpad->name),
			 "USB HIDBP drawpad %04x:%04x",
			 le16_to_cpu(dev->descriptor.idVendor),
			 le16_to_cpu(dev->descriptor.idProduct));

	usb_make_path(dev, drawpad->phys, sizeof(drawpad->phys));
	strlcat(drawpad->phys, "/input0", sizeof(drawpad->phys));

	input_dev->name = drawpad->name;
	input_dev->phys = drawpad->phys;
	usb_to_input_id(dev, &input_dev->id);
	input_dev->dev.parent = &intf->dev;

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	input_dev->keybit[BIT_WORD(BTN_drawpad)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
	input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
	input_dev->keybit[BIT_WORD(BTN_drawpad)] |= BIT_MASK(BTN_SIDE) |
		BIT_MASK(BTN_EXTRA);
	input_dev->relbit[0] |= BIT_MASK(REL_WHEEL);

	input_set_drvdata(input_dev, drawpad);

	input_dev->open = usb_drawpad_open;
	input_dev->close = usb_drawpad_close;

	usb_fill_int_urb(drawpad->irq, dev, pipe, drawpad->data,
			 (maxp > 8 ? 8 : maxp),
			 usb_drawpad_irq, drawpad, endpoint->bInterval);
	drawpad->irq->transfer_dma = drawpad->data_dma;
	drawpad->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	error = input_register_device(drawpad->dev);
	if (error)
		goto fail3;

	usb_set_intfdata(intf, drawpad);
	return 0;

fail3:	
	usb_free_urb(drawpad->irq);
fail2:	
	usb_free_coherent(dev, 8, drawpad->data, drawpad->data_dma);
fail1:	
	input_free_device(input_dev);
	kfree(drawpad);
	return error;
    */
	struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;
 
    iface_desc = intf->cur_altsetting;
    printk(KERN_INFO "Pen i/f %d now probed: (%04X:%04X)\n",
            iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
    printk(KERN_INFO "ID->bNumEndpoints: %02X\n",
            iface_desc->desc.bNumEndpoints);
    printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
            iface_desc->desc.bInterfaceClass);
 
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++)
    {
        endpoint = &iface_desc->endpoint[i].desc;
 
        printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",
                i, endpoint->bEndpointAddress);
        printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",
                i, endpoint->bmAttributes);
        printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",
                i, endpoint->wMaxPacketSize, endpoint->wMaxPacketSize);
    }
 
    //device = interface_to_usbdev(intf);
    return 0;
}


static void usb_drawpad_disconnect(struct usb_interface *intf)
{
	struct usb_drawpad *drawpad = usb_get_intfdata (intf);
    printk(KERN_INFO "----------------------\n");
    printk(KERN_INFO "input_driver disconnected!\n");
    printk(KERN_INFO "----------------------\n");

	usb_set_intfdata(intf, NULL);
	if (drawpad) {
		usb_kill_urb(drawpad->irq);
		input_unregister_device(drawpad->dev);
		usb_free_urb(drawpad->irq);
		usb_free_coherent(interface_to_usbdev(intf), 8, drawpad->data, drawpad->data_dma);
		kfree(drawpad);
	}
}


#define VENDOR_ID 0x256c
#define PRODUCT_ID 0x006d

static const struct usb_device_id usb_drawpad_id_table[] = {
    {USB_DEVICE(VENDOR_ID,PRODUCT_ID)},
	{USB_DEVICE(0x0416, 0x3f00)},
    {}
};

MODULE_DEVICE_TABLE (usb, usb_drawpad_id_table);

static struct usb_driver usb_drawpad_driver = {
	.name		= "usbdrawpad",
	.probe		= usb_drawpad_probe,
	.disconnect	= usb_drawpad_disconnect,
	.id_table	= usb_drawpad_id_table,
};

module_usb_driver(usb_drawpad_driver);