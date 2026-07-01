# RobotAuto 4WD - ZIP con estructura corregida

Este ZIP fue rearmado para que las carpetas estén visibles directamente al abrirlo.

## Orden correcto de prueba

### 1. Prueba directa sin ESP32
Abre esta carpeta:

`00_TEST_DIRECTO_UNO_NANO`

Carga este archivo en Arduino UNO/Nano:

`00_TEST_DIRECTO_UNO_NANO.ino`

Antes de cargarlo, desconecta temporalmente los cables del ESP32 en D0 y D1.

Este test no usa HTML, WiFi, ESP32, MQTT ni Serial. Solo valida Arduino + pines + puente H + motores + servo + ultrasónico.

### 2. Firmware normal del Arduino
Cuando la prueba directa funcione, carga:

`01_ARDUINO_NORMAL_CONTROL/Arduino_Normal_ControlAuto.ino`

### 3. Firmware del ESP32
Después carga:

`02_ESP32_COMUNICACION/ESP32_ControladorComunicacion.ino`

### 4. Interfaz web
La interfaz está en:

`03_WEB_HTML/page.html`

## Pines usados

- ESP32 TX GPIO17 -> Arduino D0/RX
- ESP32 RX GPIO16 <- Arduino D1/TX con divisor de tensión
- Servo -> D9
- Ultrasónico Trigger -> A2
- Ultrasónico Echo -> A3
- Delantero izquierdo: IN1 D2, ENA D3, IN2 D4
- Delantero derecho: ENB D5, IN3 D7, IN4 D8
- Trasero izquierdo: ENA D10, IN1 D12, IN2 D13
- Trasero derecho: ENB D11, IN3 A0, IN4 A1

## Diagnóstico directo

Si `00_TEST_DIRECTO_UNO_NANO` no mueve ninguna rueda, el problema no es el HTML ni el ESP32. Revisa alimentación del puente H, GND común, enable, cables de motor, batería y conexión del driver.

Si el test directo sí mueve ruedas, pero la web no mueve nada, el problema está en comunicación ESP32-Arduino o en la red/IP.
