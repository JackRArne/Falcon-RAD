import serial
import sys
import time
import os
import calendar
import binascii
from datetime import datetime
from typing import List
from functools import partial

def send_packet(return_packet):

    globals()['port'].write(return_packet)


def receive_packet():
    print(globals()['port'].in_waiting)
    if globals()['port'].in_waiting:
        reply = globals()['port'].read(40)
    else:
        reply = b'\x00\x00'

    return (reply)


for i in range(0, 255):
    package = receive_packet()
    send_packet(package)
