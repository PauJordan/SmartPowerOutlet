/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

/* Config */
#define IPANEMA_DEVICE_ID (1)

const char* ssid = "PW2G";
const char* password = "oliveras1";
const char* mqtt_server = "192.168.1.102";

#define MSG_BUFFER_SIZE  (200)
const int buf1_size = 1000;
const unsigned int Ts = 2;
const unsigned int Avg_n = 200;

const int voltage_rms_V = 230;
const int mA_per_count = 39;


/* Hardware Definitions*/
#define RELAY_PIN D0
#define CURRENT_SENSOR_PIN A0

/* Libraries */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* GLobal object instances */
WiFiClient espClient;
PubSubClient client(espClient);

/* Globals */
char msg[MSG_BUFFER_SIZE];
unsigned long long timestamp = 0; 
unsigned long int power_avg_W = 0;
unsigned long int aenergy_W_s = 0;

void reconnect() {
  // Bucle fins que ens connectem.
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "Ipanema_";
    clientId += String(IPANEMA_DEVICE_ID, HEX);
    // Provem de connectar
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("ipanema", "hello world");
      // ... i resubscribe
      client.subscribe("timestamp");
      client.subscribe("relay");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  static unsigned long last_power_msg_ticks, last_aenergy_msg_ticks = 0;
  sensor_loop();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - last_power_msg_ticks > 5000) {
    last_power_msg_ticks = now;
    snprintf (msg, MSG_BUFFER_SIZE, "{\"timestamp\":%llu,\"value\":%lu, \"id\":%u}", timestamp, power_avg_W, (unsigned int) IPANEMA_DEVICE_ID);
    Serial.print("mqtt_out: ");
    Serial.println(msg);
    client.publish("power", msg);
  }

  else if (now - last_aenergy_msg_ticks > 2000) {
    last_aenergy_msg_ticks = now;
    snprintf (msg, MSG_BUFFER_SIZE, "{\"timestamp\":%llu,\"value\":%g, \"id\":%u}", timestamp, (float) aenergy_W_s/3600, (unsigned int) IPANEMA_DEVICE_ID);
    Serial.print("mqtt_out: ");
    Serial.println(msg);
    client.publish("aenergy", msg);
  }

}
