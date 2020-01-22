As a temporary solution to usbhid claiming the drawpad before this driver has
a chance, just hit that 
sudo rmmod usbhid
before plugging the drawpad in. A more permanent solution on a per-computer
basis may be found here:
https://superuser.com/questions/1136303/unbind-device-from-usbhid-and-bind-to-your-own-driver

Also check 
/etc/udev/rules.d/
The second line (currently commented out) completely blacklists the drawpad
from the system. May be useful later, for not not at all.
