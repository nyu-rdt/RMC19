#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define SERIALSPEED 115200

#define SENSOR_BYTE 0xF2

#define WIFI_SSID "Team_15"
#define WIFI_PASS "lunabots"
#define WIFI_CHANNEL 1

#define SERVER_ADDR "192.168.1.104"
#define SERVER_PORT 1883
#define SUBSYSTEM_NAME "digging"

#define MQTT_RECONNECT_TIMEOUT 200
#define MQTT_READ_TIMEOUT 500  

int serialExpected = 0;
int readFromSensor = -1;
int serialValue = -1;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER_ADDR, SERVER_PORT, SUBSYSTEM_NAME, "");


Adafruit_MQTT_Publish rawByteOut = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/output/rawData");
Adafruit_MQTT_Subscribe rawByteIn = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/input/rawData");
Adafruit_MQTT_Subscribe motorA = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/motors/a");
Adafruit_MQTT_Subscribe motorB = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/motors/b");
Adafruit_MQTT_Subscribe motorC = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/linearActuator/a");
Adafruit_MQTT_Subscribe motorD = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/linearActuator/b");

Adafruit_MQTT_Subscribe sensorA = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/a");
Adafruit_MQTT_Subscribe sensorB = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/b");
Adafruit_MQTT_Subscribe sensorC = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/c");
Adafruit_MQTT_Subscribe sensorD = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/d");
Adafruit_MQTT_Subscribe sensorE = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/e");
Adafruit_MQTT_Subscribe sensorF = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/f");
Adafruit_MQTT_Subscribe sensorG = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/g");
Adafruit_MQTT_Subscribe sensorH = Adafruit_MQTT_Subscribe(&mqtt, SUBSYSTEM_NAME "/sensor/h");

Adafruit_MQTT_Publish sensorAPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/a");
Adafruit_MQTT_Publish sensorBPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/b");
Adafruit_MQTT_Publish sensorCPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/c");
Adafruit_MQTT_Publish sensorDPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/d");
Adafruit_MQTT_Publish sensorEPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/e");
Adafruit_MQTT_Publish sensorFPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/f");
Adafruit_MQTT_Publish sensorGPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/g");
Adafruit_MQTT_Publish sensorHPub = Adafruit_MQTT_Publish(&mqtt, SUBSYSTEM_NAME "/sensor/h");

void mqtt_connect();
void sendPreamble(int index);
void sendSensorPreamble(int index);
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
    mqtt.subscribe(&sensorA);
    mqtt.subscribe(&sensorB);
    mqtt.subscribe(&sensorC);
    mqtt.subscribe(&sensorD);
    mqtt.subscribe(&sensorE);
    mqtt.subscribe(&sensorF);
    mqtt.subscribe(&sensorG);
    mqtt.subscribe(&sensorH);
}

void loop() {
    mqtt_connect();
    Adafruit_MQTT_Subscribe* subPtr;
    while (Serial.available() > 0) {
      handleSerial();
    }
    
    while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
        if (subPtr == &motorA){
            sendPreamble(1);
            sendValue(motorA.lastread);
            sendStop();
        }
        else if (subPtr == &motorB){
            sendPreamble(2);
            sendValue(motorB.lastread);
            sendStop();
        }
        else if (subPtr == &motorC){
            sendPreamble(3);
            sendValue(motorC.lastread);
            sendStop();
        }
        else if (subPtr == &motorD){
            sendPreamble(4);
            sendValue(motorD.lastread);
            sendStop();
        }
        else if (subPtr == &sensorA){
            sendSensorPreamble(1);
            sendStop();
        }
        else if (subPtr == &sensorB){
            sendSensorPreamble(2);
            sendStop();
        }
        else if (subPtr == &sensorC){
            sendSensorPreamble(3);
            sendStop();
        }
        else if (subPtr == &sensorD){
            sendSensorPreamble(4);
            sendStop();
        }
        else if (subPtr == &sensorE){
            sendSensorPreamble(5);
            sendStop();
        }
        else if (subPtr == &sensorF){
            sendSensorPreamble(6);
            sendStop();
        }
        else if (subPtr == &sensorG){
            sendSensorPreamble(7);
            sendStop();
        }
        else if (subPtr == &sensorH){
            sendSensorPreamble(8);
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

void sendSensorPreamble(int sensor){
    Serial.write(0xF1);
    uint8_t sensor_byte = 0xFF & sensor; 
    Serial.write(sensor_byte);
}

void sendStop(){
    Serial.write(0xFF);
}

void sendValue(uint8_t* data){
    //update when we know what the data looks like over the wire
    uint8_t value = atoi((char*) data);
    Serial.write(value);
}

void handleSerial() {
  int incomingByte = 0;
  while (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (serialExpected == 0 && incomingByte == SENSOR_BYTE) {
      ++serialExpected;
    }
    else if (serialExpected == 1) {
      // Sensor Number
      readFromSensor = incomingByte;
      ++serialExpected;
    }
    else if (serialExpected == 2) {
      // Sensor Value
      serialValue = incomingByte;
      ++serialExpected;
    }
    else if (serialExpected == 3 && incomingByte == 0xFF) {
      if (readFromSensor == 1) {
        sensorAPub.publish(serialValue);
      }

      serialExpected = 0;
      readFromSensor = -1;
      serialValue = -1;
      return;
    }
  }
}
