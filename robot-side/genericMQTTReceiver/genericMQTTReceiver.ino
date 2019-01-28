#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define SERIALSPEED 115200

#define WIFI_SSID "wifi"
#define WIFI_PASS "pass"
#define WIFI_CHANNEL 1

#define SERVER_ADDR "10.100.32.167"
#define SERVER_PORT 1883
#define SUBSYSTEM_NAME "EXAMPLE"

#define MQTT_RECONNECT_TIMEOUT 200
#define MQTT_READ_TIMEOUT 50

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER_ADDR, SERVER_PORT, SUBSYSTEM_NAME, "");


Adafruit_MQTT_Publish rawByteOut = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/output/rawData");
Adafruit_MQTT_Subscribe rawByteIn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/input/rawData");

void mqtt_connect();


void setup() {
    Serial.begin(SERIALSPEED);
    delay(1);
    WiFi.begin(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
    while (WiFi.status() != WL_CONNECTED){
        delay(200);
    }
    mqtt.subscribe(&rawByteIn);

}

void loop() {
    mqtt_connect();
    Adafruit_MQTT_Subscribe* subPtr;
    while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
        if (subPtr == &rawByteIn){
          /*char* result = (char *) rawByteIn.lastread;
          uint8_t val = (uint8_t) result[0];
          int i = 1;
          while (val != 0x00){
            Serial.print(val, DEC);
            val = (uint8_t) result[i++];
          }*/
          char* holder = (char *)rawByteIn.lastread;
          int result = atoi(holder);
          Serial.print(result, DEC);
          if (result == 687){
            rawByteOut.publish("Pressed W or S");
          }
          if (result == 283){
            rawByteOut.publish("Pressed A");
          }
          if (result == 1091){
            rawByteOut.publish("Pressed D");
          }
        }
    }

    while (Serial.available() > 0){
        rawByteOut.publish(Serial.read());
    }
}

void mqtt_connect(){
    if (mqtt.connected()) return; 
    int8_t res;
    res = mqtt.connect();
    while (res != 0){
        mqtt.disconnect();
        delay(MQTT_RECONNECT_TIMEOUT);
        res = mqtt.connect();
    }
}
