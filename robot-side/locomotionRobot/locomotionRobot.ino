#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define SERIALSPEED 115200

#define WIFI_SSID "Team_15"
#define WIFI_PASS "lunabots"
#define WIFI_CHANNEL 1

#define SERVER_ADDR "192.168.1.104"
#define SERVER_PORT 1883
#define SUBSYSTEM_NAME "locomotion"

#define MQTT_RECONNECT_TIMEOUT 200
#define MQTT_READ_TIMEOUT 500  

#define FL 0x08
#define FR 0x02
#define RR 0x04
#define RL 0x10


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER_ADDR, SERVER_PORT, SUBSYSTEM_NAME, "");


Adafruit_MQTT_Publish rawByteOut = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/output/rawData");
Adafruit_MQTT_Subscribe rawByteIn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/input/rawData");
Adafruit_MQTT_Subscribe forwardMove = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/direction/forward");
Adafruit_MQTT_Subscribe leftTurn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/turn/left");
Adafruit_MQTT_Subscribe rightTurn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/turn/right");


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
    mqtt.subscribe(&forwardMove);
    mqtt.subscribe(&leftTurn);
    mqtt.subscribe(&rightTurn);

}


void forwardMoveAction(uint8_t* power){
    sendPreamble(FL);
    sendValue(power);
    sendStop();
    sendPreamble(FR);
    sendValue(power);
    sendStop();
    sendPreamble(RL);
    sendValue(power);
    sendStop();
    sendPreamble(RR);
    sendValue(power);
    sendStop();
}
void leftMoveAction(uint8_t* power){
    uint8_t value = atoi((char*)power);
    uint8_t rev = ~(value) + 1;
    sendPreamble(FL);
    sendValue(power);
    sendStop();
    sendPreamble(FR);
    sendRaw(rev);
    sendStop();
    sendPreamble(RL);
    sendValue(power);
    sendStop();
    sendPreamble(RR);
    sendRaw(rev);
    sendStop();
}

void rightMoveAction(uint8_t* power){
    uint8_t value = atoi((char*)power);
    uint8_t rev = ~(value) + 1;
    sendPreamble(FL);
    sendRaw(rev);
    sendStop();
    sendPreamble(FR);
    sendValue(power);
    sendStop();
    sendPreamble(RL);
    sendRaw(rev);
    sendStop();
    sendPreamble(RR);
    sendValue(power);
    sendStop();
}

void loop() {
    mqtt_connect();
    Adafruit_MQTT_Subscribe* subPtr;
    while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
        if (subPtr == &forwardMove){
            forwardMoveAction(forwardMove.lastread);
        }
        else if (subPtr == &leftTurn){
            leftMoveAction(leftTurn.lastread);
        }
        else if (subPtr == &rightTurn){
            rightMoveAction(rightTurn.lastread);
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
void sendRaw(uint8_t value){
    Serial.write(value);
}
