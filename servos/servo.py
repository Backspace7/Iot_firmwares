#!/usr/bin/env python
# servo.py
import RPi.GPIO as GPIO    #Importamos la libreria RPi.GPIO
import time                #Importamos time para poder usar time.sleep

GPIO.setmode(GPIO.BCM)   #Ponemos la Raspberry en modo BOARD
GPIO.setup(11,GPIO.OUT)    #Ponemos el pin 21 como salida
p = GPIO.PWM(11,50)        #Ponemos el pin 21 en modo PWM y enviamos 50 pulsos por segundo
p.start(2.5)               #Enviamos un pulso del 7.5% para centrar el servo

try :
    while True:
        try:
                state = raw_input('Enter valve State : ')
                state =  float(state)
                p.ChangeDutyCycle(state)
                time.sleep(0.25)
        except ValueError:
                state = raw_input('Enter valid State : ')
                state = float(state)

        p.ChangeDutyCycle(state)
        time.sleep(0.5)
except KeyboardInterrupt:
        p.stop()
        GPIO.cleanup()