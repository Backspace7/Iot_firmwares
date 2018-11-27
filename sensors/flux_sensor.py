#!/usr/bin/env python

import RPi.GPIO as GPIO
import time, sys

FLOW_SENSOR = 23

GPIO.setmode(GPIO.BCM)
GPIO.setup(FLOW_SENSOR, GPIO.IN, pull_up_down = GPIO.PUD_UP)
global min
min = 0
global current_count
current_count = 0
global pulsos
pulsos = 0
def countPulse(channel):
   global current_count
   current_count = current_count+1
   global pulsos
   pulsos = pulsos + 1

GPIO.add_event_detect(FLOW_SENSOR, GPIO.FALLING, callback=countPulse)
stop = 0

while True:
    stop = time.time() + 5
    current_count = 0
    while time.time() <= stop:
        try:
                time.sleep(0.1)
        except KeyboardInterrupt:
                print '\ncaught keyboard interrupt!, bye'
                GPIO.cleanup()
                sys.exit()
    if min < 15:
        min = min +1
    else:
        print('-----------')
        print ('lts/min reales:', pulsos/(7.5*60))
        min = 0
    print('pulsos en intervalo', current_count)
    print('pulsos totales', pulsos)
    print('lts/min actuales ', current_count/(7.5*60))
    print('cicle :', min)
    print('------------')