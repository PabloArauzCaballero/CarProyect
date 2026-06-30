# Robot Auto - Version corregida para Arduino IDE

Esta version esta corregida para que Arduino IDE abra todos los archivos como pestañas.

## Importante

Arduino IDE necesita que la carpeta tenga el mismo nombre que el archivo `.ino`.

Por eso ahora las carpetas son:

```txt
ESP32_ControladorComunicacion/
  ESP32_ControladorComunicacion.ino

Arduino_Normal_ControlAuto/
  Arduino_Normal_ControlAuto.ino
```

## Como abrir

### ESP32

Abre este archivo:

```txt
ESP32_ControladorComunicacion/ESP32_ControladorComunicacion.ino
```

Deberian aparecer las pestañas:

```txt
Constants.h
Globals.h
Globals.cpp
NetworkManager.h
NetworkManager.cpp
MqttManager.h
MqttManager.cpp
TelemetryManager.h
TelemetryManager.cpp
WebServerManager.h
WebServerManager.cpp
```

### Arduino normal

Abre este archivo:

```txt
Arduino_Normal_ControlAuto/Arduino_Normal_ControlAuto.ino
```

Deberian aparecer las pestañas:

```txt
Constants.h
Globals.h
Globals.cpp
SerialBridge.h
SerialBridge.cpp
MotorController.h
MotorController.cpp
ServoController.h
ServoController.cpp
DistanceSensor.h
DistanceSensor.cpp
TelemetryManager.h
TelemetryManager.cpp
```

## Conexion ESP32 con Arduino normal

```txt
ESP32 GPIO17 / TX2  ->  Arduino pin 2 / RX SoftwareSerial
ESP32 GPIO16 / RX2  <-  Arduino pin 3 / TX SoftwareSerial
ESP32 GND           <-> Arduino GND
```

Si el Arduino normal es de 5V, protege esta linea con divisor de voltaje:

```txt
Arduino pin 3 / TX -> ESP32 GPIO16 / RX2
```

## Configuracion que debes cambiar

En el ESP32:

```txt
ESP32_ControladorComunicacion/Constants.h
```

Cambia:

```cpp
static const char WIFI_SSID[]     = "NOMBRE_WIFI";
static const char WIFI_PASSWORD[] = "CLAVE_WIFI";
static const char MQTT_BROKER_IP[] = "192.168.137.1";
```

En el Arduino normal:

```txt
Arduino_Normal_ControlAuto/Constants.h
```

Cambia los pines de motor, servo y sensor si tu conexion fisica es distinta.
