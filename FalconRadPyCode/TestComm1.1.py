import serial
import sys
import glob
import sched
import time
import os
import calendar
import binascii
from datetime import datetime
from typing import List
from functools import partial

def serial_ports():

    globals()['port'] = serial.Serial('COM4',115200,timeout = 0.1)



def send_packet():
    #reqpacket = [0xFF,0xAA]

    reqpacket_1 = [0x15,0x10,0x01,0x00]
    globals()['port'].write(reqpacket_1)
    print("packet sent")
    print(reqpacket_1)
    time.sleep(2)

    
def receive_packet():
    print(globals()['port'].in_waiting)
    if globals()['port'].in_waiting:
        reply = globals()['port'].read(20) 
        print(reply)
        return(reply)
    else:
        print('no')
        
        
    
serial_ports()
for i in range (0,255):
    send_packet()
    package = receive_packet()
    
