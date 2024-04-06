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

    globals()['port'] = serial.Serial('COM3',115200, timeout = 0.1)

