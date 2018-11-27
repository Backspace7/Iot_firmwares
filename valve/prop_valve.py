#!/usr/bin/env python
# servo.py
import RPi.GPIO as GPIO    #Importamos la libreria RPi.GPIO
import time                #Importamos time para poder usar time.sleep

GPIO.setmode(GPIO.BCM)   #Ponemos la Raspberry en modo BOARD
GPIO.setup(11,GPIO.OUT)    #Ponemos el pin 21 como salida
p = GPIO.PWM(11,50)        #Ponemos el pin 21 en modo PWM y enviamos 50 pulsos por segundo
p.start(3)               #Enviamos un pulso del 7.5% para centrar el servo

state = 3

try :
    temperature = raw_input('Enter temperature :')
    temperature = float(temperature)
    limsup = temperature + 3
    liminf = temperature - 3
    p.ChangeDutyCycle(3)
    while True:
        try:
                tempfile = open("/sys/bus/w1/devices/28-020e9177de9d/w1_slave")
                thetext = tempfile.read()
                tempfile.close()
                tempdata = thetext.split("\n")[1].split(" ")[9]
                temp = float(tempdata[2:])
                temp = temp / 1000
                print('Temperature : %s'%temp)
                if temp <= liminf and state <9:
                        state = state +1
                        p.ChangeDutyCycle(state)
                        time.sleep(1)
                elif temp >= limsup and state > 3:
                        state = state -1
                        p.ChangeDutyCycle(state)
                        time.sleep(1)
                else:
                        time.sleep(1)
        except KeyboardInterrupt:
               p.stop()
               GPIO.cleanup()

        p.ChangeDutyCycle(state)
        time.sleep(0.5)
except ValueError:
        temperature = raw_input('Enter a valid temperature :')
	temperature = float(temperature)
        limsup = temperature + 3
        liminf = temperature - 3

p.stop()
GPIO.cleanup()