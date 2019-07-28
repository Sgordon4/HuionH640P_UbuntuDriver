#!/usr/bin/env python3
import struct
PRINT_TIMESTAMP = True
# Open the file in the read-binary mode
#f = open("/dev/input/by-id/usb-VEIKK.INC_A30_0000001-event-mouse", "rb" )
f = open("/dev/input/by-id/usb-256c_006d-if01-event-mouse", "rb" )

while 1:
    data = f.read(24)
    # print struct.unpack('4IHHI',data)
    ###### FORMAT = ( Time Stamp_INT , 0 , Time Stamp_DEC , 0 , 
    ######   type , code ( key pressed ) , value (press/release) )
    time_int, _, time_dec, _, ev_type, ev_code, ev_val = struct.unpack('4IHHI',data)
    t = (ev_type, ev_code)
    if ((t == (0,0) and ev_val == 0)
        or (t == (4, 4) and ev_val >= 589825 and ev_val <= 589827)):
      # Redundant as it's for normal/bottom/top clicks
      # (same code for press/release), or just garbage 0,0,0
        continue
    if PRINT_TIMESTAMP:
        print("[{:.2f}] ".format(time_int + time_dec/1e6),
              end="", flush=True)
    if t == (3,0):
        print("Pos x: {} ({:.2f}%)".format(ev_val, 100*ev_val/32767), flush=True)
    elif t == (3,1):
        print("Pos y: {} ({:.2f}%)".format(ev_val, 100*ev_val/32767), flush=True)
    elif t == (3,24):
        print("Pression: {} ({:.2f}%)".format(ev_val, 100*ev_val/8191), flush=True)
    elif t == (1,272):
        print("Normal click ({})".format("press" if ev_val else "release"), flush=True)
    elif t == (1,273):
        print("click button 2 (bottom) ({})".format("press" if ev_val else "release"), flush=True)
    elif t == (1,274):
        print("click button 3 (top) ({})".format("press" if ev_val else "release"), flush=True)
    else:
        print("Unknow: type={}, code={}, value={}".format(ev_type, ev_code, ev_val), flush=True)
