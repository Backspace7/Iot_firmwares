## Config For therm sensor in Raspberry
#  Use 4ohm7 resistor between Signal and VCC 3v3

  sudo nano /boot/config.txt
# In the end of file add
  # 1-wire settings
  dtoverlay=w1-gpio,gpiopin=4
# Ejecute this command
  sudo modprobe w1-gpio
  sudo modprobe w1-therm
