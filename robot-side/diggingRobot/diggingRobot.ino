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
Adafruit_MQTT_Subscribe motorA = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/digging/motors/a");
Adafruit_MQTT_Subscribe motorB = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/digging/motors/b");
Adafruit_MQTT_Subscribe motorC = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/digging/motors/c");
Adafruit_MQTT_Subscribe motorD = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/digging/motors/d");


void mqtt_connect();
void sendPreamble(int index);
void sendValue(void* data);
void sendStop();

void setup() {
    Serial.begin(SERIALSPEED);
    delay(1);
    WiFi.begin(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
    while (WiFi.status() != WL_CONNECTED){
        delay(200);
    }
    mqtt.subscribe(&rawByteIn);
    mqtt.subscribe(&motorA);
    mqtt.subscribe(&motorB);
    mqtt.subscribe(&motorC);
    mqtt.subscribe(&motorD);

}

void loop() {
    mqtt_connect();
    Adafruit_MQTT_Subscribe* subPtr;
    while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
        if (subPtr == motorA){
            sendPreamble(0);
            sendValue(motorA.lastRead);
            sendStop();
        }
        else if (subPtr == motorB){
            sendPreamble(1);
            sendValue(motorB.lastRead);
            sendStop();
        }
        else if (subPtr == motorC){
            sendPreamble(2);
            sendValue(motorC.lastRead);
            sendStop();
        }
        else if (subPtr == motorD){
            sendPreamble(3);
            sendValue(motorD.lastRead);
            sendStop();
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
void sendPreamble(int motor){
    Serial.write(0xF0);
    uint8_t motor_byte = 0xFF & motor; 
    Serial.write(motor_byte);
}

void sendStop(){
    Serial.write(0xFF);
}

void sendValue(void* data){
    //update when we know what the data looks like over the wire
    int value = *((int*)data);
    Serial.write(value);
}
