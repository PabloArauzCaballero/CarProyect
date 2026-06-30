#include "Constants.h"
#include "Globals.h"

int motor_der = 0;
int motor_izq = 0;
int servo = 90;
int distancia = 0;

unsigned long lastMsg = 0;
unsigned long lastMqttReconnect = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiServer webServer(WEB_SERVER_PORT);
