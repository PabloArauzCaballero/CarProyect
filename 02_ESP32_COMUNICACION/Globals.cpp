#include "Constants.h"
#include "Globals.h"

int motor_der = 0;
int motor_izq = 0;
int motor_izq_del = 0;
int motor_der_del = 0;
int motor_izq_tras = 0;
int motor_der_tras = 0;
int servo = 90;
int distancia = 0;

unsigned long lastMsg = 0;
unsigned long lastMqttReconnect = 0;

bool telemetryOk = false;
String lastTelemetryRaw = "";
unsigned long lastTelemetryReceivedAt = 0;
unsigned long telemetryParseErrorCount = 0;
unsigned long telemetryIgnoredByteCount = 0;
String lastSerialSample = "";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiServer webServer(WEB_SERVER_PORT);
