# Sensor Gas MQ-4
El sensor se conecta al ESP32 por medio de los pines analogicos y digital,  12 y 13 respectivamente. Detecta la presencia de GAS por medio del pin digital y envia el nivel de voltaje de acuerdo a la concentracion de gas por el pin analogico.
No discrimina en el tipo de gas detectado, solo detecta la concentracion en un ambiente con aire limpio, la sensibilidad se ajusta por medio del potenciometro del sensor, actualmente calibrado a 900.
#### Configuración para sensores
  - sensor MQ-4
  - VCC de 5V
  - Cable A0 al G12
  - Cable D0 al G13


#### set-up
    - se conecta VCC del sensor a 5V
    - GND sensores a GND
    - A0 se conecta al G12
    - D0 se conecta al G13
