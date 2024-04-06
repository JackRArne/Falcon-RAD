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

    reqpacket_1 = b'\x12\x12'            #create initial packet
    icsum = binascii.crc32(reqpacket_1)  #checksum calculation of the packet
    
    #packet_final = [reqpacket_1]
    #globals()['port'].write(reqpacket_1)

    icsumb = icsum.to_bytes(4,'big')
    print(icsumb)                         #convert checksum to bytes
    packet_final = reqpacket_1+icsumb     #make one list of bytes with both the packet at checksum bytes
    globals()['port'].write(packet_final) #write the pakcet to the arduino
    print("packet sent")                
    print(packet_final)                   #print the packet
    time.sleep(1)                         #have a one second wait period
    return(icsum)
    
    
def receive_packet():
    print(globals()['port'].in_waiting)   #print the amount of bytes waiting
    if globals()['port'].in_waiting:      #if there are bytes waiting 
        reply = globals()['port'].read(20) #read the reply from arduino
        print(reply)                      #print the reply from arduino
        return(reply)
    else:
        print('no')                 #if no bytes are waiting print no
        
        
    
serial_ports()
for i in range (0,255):
    if globals()['port'].in_waiting == 0: # to make sure we don't overfill the arduino
        send_packet()
    if globals()['port'].in_waiting != 0:
        package = receive_packet()


        ics = send_packet()
        final = package[:2]
        csn = package[2:]
        csni = int.from_bytes(csn,byteorder = 'big')
        fcs = binascii.crc32(final)
        Check = fcs-csni
        print(ics)
        print(Check)
        if Check ==0:       #check to make sure the checksums match
            print('CHECKSUM GOOD')
        else:
            print('CHECKSUM BAD')
        print('\n')
    


