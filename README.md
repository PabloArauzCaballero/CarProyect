# CarProyect D10/D13 BAUD9600 ROBUSTO

Versión ajustada para tu cableado actual:

```text
ESP32 RX2 / GPIO16  <-  Nano D10  (TX SoftwareSerial del Nano)
ESP32 TX2 / GPIO17  ->  Nano D13  (RX SoftwareSerial del Nano)
GND                 <-> GND
```

## Cambio importante

La comunicación Nano <-> ESP32 queda en **9600 baudios** porque Arduino Nano con `SoftwareSerial` en D10/D13 no es estable a 115200.

```cpp
// Nano
ESP32_SERIAL_BAUD = 9600

// ESP32
ARDUINO_SERIAL_BAUD = 9600
```

El Monitor Serial del ESP32 sigue en **115200**.

## Qué corrige esta versión

El ESP32 ahora acepta los dos formatos de telemetría:

```text
<0,0,90,7>
0,0,90,7
```

Esto es intencional: si el Nano quedó con el sketch anterior y todavía manda CSV plano, el ESP32 igual podrá leerlo. Además `/telemetry` ahora incluye:

```json
"ignored_bytes", "serial_sample", "last_raw"
```

Si no hay telemetría válida, `serial_sample` mostrará qué bytes reales están entrando al RX2 del ESP32.

## Prueba

1. Sube el sketch del Nano.
2. Sube el sketch del ESP32.
3. Abre Monitor Serial del ESP32 a 115200.
4. Entra a `http://IP_DEL_ESP32/telemetry`.

Salida esperada:

```json
{"motor_der":0,"motor_izq":0,"servo":90,"distancia":7,"telemetry_ok":true}
```

Si `telemetry_ok` sigue en false, revisa el campo `serial_sample`.

- Si ves algo como `0,0,90,7`, el ESP32 ya está leyendo CSV plano y esta versión debería parsearlo.
- Si ves caracteres raros, el problema es señal física/ruido/baud o falta de GND común.
- Si `serial_sample` está vacío, el ESP32 no está recibiendo nada desde el Nano.
