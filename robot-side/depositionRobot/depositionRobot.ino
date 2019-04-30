#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define SERIALSPEED 115200

#define WIFI_SSID "Team_15"
#define WIFI_PASS "lunabots"
#define WIFI_CHANNEL 1

#define SERVER_ADDR "192.168.1.104"
#define SERVER_PORT 1883
#define SUBSYSTEM_NAME "deposition"

#define MQTT_RECONNECT_TIMEOUT 200
#define MQTT_READ_TIMEOUT 500  

#define ACTUATOR 0x01
#define LM 0x02
#define RM 0x08
#define VIBRATION 0x40

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER_ADDR, SERVER_PORT, SUBSYSTEM_NAME, "");


Adafruit_MQTT_Publish rawByteOut = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/output/rawData");
Adafruit_MQTT_Subscribe rawByteIn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/input/rawData");
Adafruit_MQTT_Subscribe motorA = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/motors/a");
Adafruit_MQTT_Subscribe motorB = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/motors/b");
Adafruit_MQTT_Subscribe linearActuator = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/linearActuator/a");
Adafruit_MQTT_Subscribe vibration = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/vibration/a");


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
    mqtt.subscribe(&linearActuator);
    mqtt.subscribe(&vibration);

}

void loop() {
    mqtt_connect();
    Adafruit_MQTT_Subscribe* subPtr;
    while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
        if (subPtr == &motorA){
            sendPreamble(LM);
            sendValue(motorA.lastread);
            sendStop();
        }
        else if (subPtr == &motorB){
            sendPreamble(RM);
            sendValue(motorB.lastread);
            sendStop();
        }
        else if (subPtr == &linearActuator){
            sendPreamble(ACTUATOR);
            sendValue(linearActuator.lastread);
            sendStop();
        }
        else if (subPtr == &vibration){
            sendPreamble(VIBRATION);
            sendValue(vibration.lastread);
            sendStop();
        }
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

void sendValue(uint8_t* data){
    //update when we know what the data looks like over the wire
    uint8_t value = atoi((char*) data);
    Serial.write(value);

}
