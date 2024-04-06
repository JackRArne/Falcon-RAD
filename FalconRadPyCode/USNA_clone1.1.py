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

    globals()['port'] = serial.Serial('COM4',115200,timeout = 0.1,
                                      bytesize=serial.EIGHTBITS,
                                      stopbits=serial.STOPBITS_ONE,
                                      parity=serial.PARITY_NONE)



def send_packet():
    reqpacket = bytearray(12)
    reqpacket[:2] = (255,255)
    reqpacket[2:10] = (170,170,187,187,170,170,187,187,170,170)
    reqpacket[10:12] = (255,255)
    globals()['port'].write(reqpacket) #writes out twelve bytes to the port
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
for i in range (0,10):
    send_packet()
    #time.sleep(0.1)
    receive_packet()
    time.sleep(1)
    print('\n')
    print('\n')
    
