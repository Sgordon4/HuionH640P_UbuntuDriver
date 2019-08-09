
//Called when a device registered in the above 'device_table' table is plugged in
static int drawpad_probe(struct usb_interface *interface, const struct usb_device_id *id);

static void drawpad_disconnect(struct usb_interface *interface);

static int __init usb_drawpad_init(void);
static void __exit usb_drawpad_exit(void);
