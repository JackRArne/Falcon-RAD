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

    globals()['port'] = serial.Serial('COM3',115200,timeout = 0.1,
                                      bytesize=serial.EIGHTBITS,
                                      stopbits=serial.STOPBITS_ONE,
                                      parity=serial.PARITY_NONE)



def send_packet(i):
##    header_sync_pattern = 0xFFFF #sync at beginning
##    restofpacket = 0xAAAAFFFF
    reqpacket = bytearray(1)
##    for i in range (0,255):
##        byte = i
##        print(i,byte)
    reqpacket[0] = i
        #byte = byte +i
        #i= i+1
    #reqpacket[0]= 0xFF
    #reqpacket[9] = 0xFF
####    reqpacket[:2] = header_sync_pattern
####    reqpacket[2:6] = 0xAAAAAAAA
####    reqpacket[6:10] = restofpacket
    globals()['port'].write(reqpacket) #writes out ten bytes to the port
    #print(a)
    print(reqpacket)

    
def receive_packet():
    if globals()['port'].in_waiting:
        reply = globals()['port'].read(2) 
        print(reply)
        
    data_ready = True 
    if data_ready:
        #FPLogFileMessages = 'C:\Users\DF_Local\Desktop\DATA\Testv1.txt'
        FPLogFileMessages = 'test.txt'
        #with open(FPLogFileMessages,'a')as file:
            #text = reply
            #file.write(reply)
    data_ready = False
    
serial_ports()
for i in range (0,255):
    send_packet(i)
    #time.sleep(0.1)
    receive_packet()
    time.sleep(1)
    print('\n')
    print('\n')
    
