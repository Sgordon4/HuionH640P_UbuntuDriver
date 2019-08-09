#!/usr/bin/env python3

#################################################
# The goal of this script is to output a device #
# file for testing/development purposes,        #
# created to assist with the development of a   #
# linux driver for the Huion H640P drawing pad  #
#################################################

import os.path
import struct
from enum import Enum

#####################################################
#Tuples defining different 'codes' the device outputs
#Dictionary explaining each type is at bottom of file
#Corresponds to (ev_type, ev_code) below

RESET = (0, 0  )

HOVER = (1, 320)
TOUCH = (1, 330)

BTN_1 = (1, 331)
BTN_2 = (9, 999)

#Pos origin is at top left corner
POS_X = (3, 0  )        # PosX max = 32767, stops 40% of the way across pad (not reading all data?)
POS_Y = (3, 1  )        # PosY max = 32767

PRESSION = (3, 24)

REDUNDANT = (4, 4)


#Corresponds to values of ev_val below
ON = 1
OFF = 0

#####################################################



PRINT_TIMESTAMP = True
FILE1 = "/dev/input/by-id/usb-256c_006d-if01-event-mouse"
FILE2 = "/dev/hidraw2"


#Open the file in read-binary mode
f = open(FILE1, "rb")

try:
    while 1:
        #Read the next set of data from the file
        data = f.read(24)

        #Unpack the data as a struct
        time_int, _, time_dec, _, ev_type, ev_code, ev_val = struct.unpack('4IHHI',data)



        #time_int and time_dec can be combined into a timestamp
        timestamp = "[{:.2f}]".format(time_int + time_dec/1e6)

        #Package together the 'code' for ease of use
        code = (ev_type, ev_code)
        
        #If statement to decide what to print, for debugging
        #Comment out a condition to print its respective value
        if( 1
        
            and code != RESET
            and code != HOVER
            and code != TOUCH
            and code != BTN_1
            and code != BTN_2
            #and code != POS_X
            and code != POS_Y
            and code != PRESSION

            and code != REDUNDANT
        ):
            if(PRINT_TIMESTAMP):
                print(timestamp, end=" ", flush=True)

            print("("+str(ev_type)                 + ", ", end="", flush=True)
            print('{:<3}'.format(str(ev_code)) + "), ", end="", flush=True)
            print(ev_val, end="", flush=True)
            
            print()
        

        



        '''
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
            '''

finally:
    f.close()



'''

#file_exists = os.path.isfile(FILE_BASE + FILE_NAME)
file_exists = os.path.isfile("/dev/input/by-id/usb-256c_006d-if01-event-mouse")
if(file_exists):
    #Open the file in read-binary mode
    f = open(FILE_BASE + FILE_NAME, "rb")

    try:
        while 1:
            #Read the next set of data from the file
            data = f.read(24)

            #Unpack the data as a struct
            print(data)

    finally:
        f.close()
else:
    print("File \"" + FILE_BASE+FILE_NAME + "\" not found!")
    '''


#########################################################
# This is a list of the definitions for different codes
# The values in parenthesis below correspond to 'code'
# 
# 'RESET'
# if (0, 0), ev_val = 0
# This marks the end of a set of updates,
# akin to a newline.
#
#
# 'HOVER_ON'
# if (1, 320), ev_val = 1
# This corresponds to the pen coming in range of the pad,
# and will always be the first line output.
# 
# 'HOVER_OFF'
# if (1, 320), ev_val = 0
# This corresponds to the pen coming out of range of the 
# pad.
#
#
# 'TOUCH_ON'
# if (1, 330), ev_val = 1
# This corresponds to the pen commencing physical contact
# with the pad.
# 
# 'TOUCH_OFF'
# if (1, 330), ev_val = 0
# This corresponds to the pen ceasing physical contact 
# with the pad.
#
#
# 'BTN_1_ON'
# if (1, 331), ev_val = 1
# This corresponds to the first button on the pen being
# pressed.
#
# 'BTN_1_OFF'
# if (1, 331), ev_val = 0
# This corresponds to the first button on the pen being
# released.
#
#
#                ########################                
# This operation conflicts with 'TOUCH_ON/OFF' for some
# reason, so in the actual enum I have assigned it a 
# meaningless, arbitrary value so it remains unused.
# (Does nothing when pressed, but when released throws a
#  TOUCH_ON, two RESETs, and a TOUCH_OFF sequentially)
# 
# 'BTN_2_ON'
# if (1, 330), ev_val = 1
# This corresponds to the second button on the pen being
# pressed.
#
# 'BTN_2_OFF'
# if (1, 330), ev_val = 0
# This corresponds to the second button on the pen being
# released.
#                ########################                
#
#
# 'POS_X'
# if (3, 0), ev_val = posX
# 
# 'POS_Y'
# if (3, 1), ev_val = posY
#
#
# 'PRESSION'
# if (3, 24), ev_val = pression
#
#
# 'REDUNDANT'
# if (4, 4), ev_val = 852034
# This code is redundant, identical to that of 'TOUCH',
# but without the difference between 'ON/OFF'
#
#########################################################