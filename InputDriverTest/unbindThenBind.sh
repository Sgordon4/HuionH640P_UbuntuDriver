echo 2-2:1.0 | sudo tee -a /sys/bus/usb/drivers/usbhid/unbind
echo 2-2:1.1 | sudo tee -a /sys/bus/usb/drivers/usbhid/unbind

echo 2-2:1.0 | sudo tee -a /sys/bus/usb/drivers/usbdrawpad/bind
echo 2-2:1.1 | sudo tee -a /sys/bus/usb/drivers/usbdrawpad/bind
