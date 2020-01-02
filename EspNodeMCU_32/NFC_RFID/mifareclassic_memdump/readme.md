# PN532
La lectura de tarjeta para posterior configuracion se hace por medio del chip PN532, este chip leera la tarjeta, abrira el sector y los bloques para obtener los datos de la tarjeta.

#### Configuración para sensores
  - VCC 5V
  - REsistencia 220ohm
  - cables

#### set-up
    - VCC va a 5V del sensor al igual que GND (se puede alimentar de forma externa)
    - MOSI va al pin G23
    - MISO se conecta a G19
    - SS se conecta a G5
    - SCK se conecta a G18