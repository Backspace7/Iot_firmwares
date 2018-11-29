#!/usr/bin/env python
# servo.py
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(11,GPIO.OUT)
p = GPIO.PWM(11,50)
p.start(3)
state = 3
while true:
    try :
        temperature = raw_input('Enter temperature :') #debiera obtenerla del websocket o servidor
        temperature = float(temperature)
        limsup = temperature + 3
        liminf = temperature - 3
        p.ChangeDutyCycle(3)
        while True:
            try:
                    tempfile = open("/sys/bus/w1/devices/28-020e9177de9d/w1_slave") # Agregar segundo sensor que mida la temperatura del termo
                    thetext = tempfile.read()
                    tempfile.close()
                    tempdata = thetext.split("\n")[1].split(" ")[9]
                    temp = float(tempdata[2:])
                    temp = temp / 1000
                    print('Temperature : %s'%temp) # enviar temperatura del agua al servidor, seria la temperatura que esta calentando
                    if temp <= liminf and state <9:
                            state = state +1
                            p.ChangeDutyCycle(state)
                            time.sleep(1)   # tiempo que se duerme hasta que toma la siguiente temperatura y mueve la valvula debiera ser 3 seg
                    elif temp >= limsup and state > 3.5:
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
            continue
    break

p.stop()
GPIO.cleanup()
