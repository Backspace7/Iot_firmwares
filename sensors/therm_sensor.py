#!/usr/bin/env python
#temp.py
import time

try:
        while True:
                tempfile = open("/sys/bus/w1/devices/28-020e9177de9d/w1_slave")
                thetext = tempfile.read()
                tempfile.close()
                tempdata = thetext.split("\n")[1].split(" ")[9]
                temperature = float(tempdata[2:])
                temperature = temperature / 1000
                print temperature

                time.sleep(1)
except KeyboardInterrupt:
        pass