#include <Servo.h>

// These are the definiions for the phase numbers
#define READ_PHASE_NO         0
#define PARSE_PHASE_NO        1
#define EXEC_PHASE_NO         2
#define WRITE_PHASE_NO        3

// Include general communications protocol definitions
// This file defines constants for communications
#define START_BYTE_MOTOR        0xF0
#define START_BYTE_SENSOR       0xF1
#define RESPONSE_BYTE_SENSOR    0xF2
#define PANIC_BYTE              0xF3
#define STOP_BYTE               0xFF

// Constants for read phase (parameters)
#define READ_BUFFER_LEN       256
#define READ_TIMEOUT_MILLIS   100    

// constants for parse phase (define byte types)
#define MOTOR_TYPE            1
#define SENSOR_TYPE           2
#define VALUE_TYPE            3
#define STOP_TYPE             4
#define START_TYPE            5

#define SENSOR_DISABLE        0
#define SENSOR_ENABLE         1
#define MOTOR_STOP            127
#define MOTOR_FULL_REVERSE    0
#define MOTOR_FULL_FORWARD    0

// Serial Definitions
#define ESP_BAUD             115200
#define ROBOTEQ_BAUD          115200

// Serial Port Mappings
HardwareSerial ESP = Serial1; // ESP WiFi Module
HardwareSerial BlueMotor = Serial3;
HardwareSerial PurpleMotor = Serial3;
HardwareSerial GoldMotor = Serial2;
HardwareSerial BlackMotor = Serial2;

// Pin Mappings
// Motors
int GreenMotorPWM = 3;
int PurpleMotorPWM = 4;
int BlueMotorPWM = 4;
int GoldMotorPWM = 5;
int BlackMotorPWM = 5;
int VibrationMotorEnable = 20;
int ServoPWM = 23;
// Sensors
int LinearPot = 14;
int Pullup1 = 15;
int Pullup2 = 16;
int Pullup4 = 25;
int Pullup5 = 26;
int MiscSensor = A14;
// Misc
int DebugLED = 13;

// Servo Definition
Servo ServoCtrl;

// DRUM PIN MAPPINGS
// Include drum protocol definitions
// Drum protocol definitions
#define DRUM_MOTOR_1_COMM       0x01
#define DRUM_MOTOR_2_COMM       0x02
#define LINEAR_ACTUATOR_COMM    0x04
#define VIBRATION_MOTOR_COMM    0x08
#define DOOR_ACTUATOR_COMM      0x10

#define LINEAR_POT_COMM         0x01
#define MAGNETIC_1_COMM         0x02
#define MAGNETIC_2_COMM         0x04
#define WEIGHT_COMM             0x08
#define DRUMENC_1_COMM          0x10
#define DRUMENC_2_COMM          0x20
#define TOP_LIMIT_COMM          0x40
#define BOTTOM_LIMIT_COMM       0x80

// Motor lookup
#define MOTOR_LOOKUP
#define MOTOR_COMM_WIDTH        DOOR_ACTUATOR_COMM + 1

void *motorPinlookup[MOTOR_COMM_WIDTH];

// Sensor lookup
#define SENSOR_LOOKUP
#define SENSOR_COMM_WIDTH       BOTTOM_LIMIT_COMM + 1

void *sensorPinlookup[SENSOR_COMM_WIDTH]; 

// Pin mappings
void initializeDrumMappings();
////////////////////



// Globals
int oPhase = 0;

// Buffers and end-of-buffer pointers
int readBuffer[READ_BUFFER_LEN];
int readBufferEnd = 0;
int motorExecBuffer[MOTOR_COMM_WIDTH];
int sensorExecBuffer[SENSOR_COMM_WIDTH];
int sensorValueBuffer[SENSOR_COMM_WIDTH];

// Function prototypes
void readPhase();
void parsePhase();
void execPhase();
void writePhase();
/////////////////


// ARDUINO FUNCTIONS
void setup() {
  // start serial ports
  ESP.begin(ESP_BAUD);
  BlueMotor.begin(ROBOTEQ_BAUD);
  GoldMotor.begin(ROBOTEQ_BAUD);

  // Pin modes
  pinMode(Pullup1, INPUT);
  pinMode(Pullup2, INPUT);
  pinMode(Pullup4, INPUT);
  pinMode(Pullup5, INPUT);
  pinMode(DebugLED, OUTPUT);
  pinMode(VibrationMotorEnable, OUTPUT);

  ServoCtrl.attach(ServoPWM);
  
  // Initialize motorExecBuffer to all 127
  memset(motorExecBuffer, MOTOR_STOP, MOTOR_COMM_WIDTH);
  // Initialize sensorExecBuffer to all 0
  memset(sensorExecBuffer, SENSOR_DISABLE, SENSOR_COMM_WIDTH);

  initializeDrumMappings();
  Serial2.begin(115200);
  Serial2.write(0x55);
}

void loop() {
  //Serial2.write(0xff);
  // main loop to switch between phases
  if (oPhase == READ_PHASE_NO) {
    readPhase();
    oPhase = PARSE_PHASE_NO;
  }
  else if (oPhase == PARSE_PHASE_NO) {
    parsePhase();
    oPhase = EXEC_PHASE_NO;
  }
  else if (oPhase == EXEC_PHASE_NO) {
    execPhase();
    oPhase = WRITE_PHASE_NO;
  }
  else if (oPhase == WRITE_PHASE_NO) {
    writePhase();
    oPhase = READ_PHASE_NO;
  }
}

// This phase reads data from the ESP into a buffer
// It ends once the buffer is full OR after a timeout
void readPhase() {
  int starttime = millis(); // when the phase started
  int current = millis(); // current time of the phase

  while (current - starttime < READ_TIMEOUT_MILLIS) {
    // If there is a character to read
    if (ESP.available() > 0) {
      // Write to end of the readBuffer
      
      readBuffer[readBufferEnd] = ESP.read();
      //Serial2.write(readBuffer[readBufferEnd]);
      ++readBufferEnd;

      // Exit if the readbuffer is full
      if (readBufferEnd > READ_BUFFER_LEN) {
        break;
      }
      current = millis();
    }
  } 
}

// This phase will take the received commands from the ESP and extracts the commands
// for each motor
void parsePhase() {
  Serial2.write(0x55);
  int nextByteType = 0; // What is the next byte to expect?
  int motorIdentifier = 0; // What motor to expect a value for?
  // variables for recovering incomplete segments
  int endOfSegment = 0;
  int incompleteSegmentStart = 0;

  // For each byte in the read buffer
  for(int a = 0; a < readBufferEnd; a++) {
    if (nextByteType == START_TYPE && readBuffer[a] == START_BYTE_MOTOR ) {
      nextByteType = MOTOR_TYPE;
    }
    else if (nextByteType == START_TYPE && readBuffer[a] == START_BYTE_SENSOR) {
      nextByteType = SENSOR_TYPE;
    }
    else if (nextByteType == MOTOR_TYPE) {
      motorIdentifier = readBuffer[a];
      nextByteType = VALUE_TYPE;
    }
    else if (nextByteType == SENSOR_TYPE) {
      // Enable sensor read
      sensorExecBuffer[readBuffer[a]] = SENSOR_ENABLE;
      nextByteType = STOP_TYPE;
    }
    else if (nextByteType == VALUE_TYPE) {
      // Set motor speed
      motorExecBuffer[motorIdentifier] = readBuffer[a];
      nextByteType = STOP_TYPE;
    }
    else if (nextByteType == STOP_TYPE && readBuffer[a] == STOP_BYTE){
      nextByteType = START_TYPE;
    }
  }

  // Check for incomplete segments
  if (nextByteType  == START_TYPE) { // The last byte received was STOP_BYTE
    readBufferEnd = 0; // Clear the buffer
  }
  else {
    // read backwards until START_BYTE
    for(int a = readBufferEnd - 1; a >= 0; a--) {
      if(readBuffer[a] == START_BYTE_MOTOR || readBuffer[a] == START_BYTE_SENSOR){
        incompleteSegmentStart = a;
        break;
      }
    }

    // copy data to beginning of buffer
    endOfSegment = 0;
    for (int c = incompleteSegmentStart; c < readBufferEnd; c++){
      readBuffer[endOfSegment] = readBuffer[c];
      ++endOfSegment;
    }

    // move read buffer end pointer to end of copied segment
    readBufferEnd = endOfSegment;
  }
}

// Execute phase writes commands to actual electronics
void execPhase() {  
  for (int i = 1; i < MOTOR_COMM_WIDTH; i = i << 1) {
    if ((HardwareSerial *)motorPinlookup[i] == &BlueMotor || (HardwareSerial *)motorPinlookup[i] == &GoldMotor) {
      // This is a SerialPort, gotta do something with that later
      continue;
    }
    else if ((Servo *)motorPinlookup[i] == &ServoCtrl) {
      // This is a Servo motor, gotta do something with that later
      continue;
    }
    analogWrite(*(int *)motorPinlookup[i], motorExecBuffer[i]);
] }

  for (int i = 1; i < SENSOR_COMM_WIDTH; i = i << 1) {
    if (sensorExecBuffer[i] == SENSOR_ENABLE) {
      if (*(int *)sensorPinlookup[i] > A0) { // Analog pin
        sensorValueBuffer[i] = analogRead(*(int *)sensorPinlookup[i]);
      }
      else if (*(HardwareSerial *)sensorPinlookup[i] == GoldMotor || *(HardwareSerial *)sensorPinlookup[i] == BlueMotor) {
        // Serial port, gotta do this
      }
      else {
        sensorValueBuffer[i] = digitalRead(*(int *)sensorPinlookup[i]);
      }
    }
  }
}

// This phase write the collected sensor values to the ESP module
void writePhase() {
  int unscaled;
  
  for (int i = 1; i < SENSOR_COMM_WIDTH; i = i << 1) {
    if (sensorExecBuffer[i] == SENSOR_ENABLE) {
      unscaled = sensorValueBuffer[i]; // TODO: unscale value from 10-bits to 8-bits
      
      Serial1.write(RESPONSE_BYTE_SENSOR); // Start byte indicating sensor value
      Serial1.write(i); // The sensor identifier
      Serial1.write(unscaled); // The value read from the sensor
      Serial1.write(STOP_BYTE); // End of packet

      sensorExecBuffer[i] = SENSOR_DISABLE;
    }
  }
}


// ROVER DEFINITIONS
void initializeDrumMappings() {
  motorPinlookup[DRUM_MOTOR_1_COMM] = (void *)&BlueMotorPWM; // If using PWM, replace with BlueMotorPWM
  motorPinlookup[DRUM_MOTOR_2_COMM] = (void *)&GoldMotorPWM;
  motorPinlookup[LINEAR_ACTUATOR_COMM] = (void *)&GreenMotorPWM;
  motorPinlookup[VIBRATION_MOTOR_COMM] = (void *)&VibrationMotorEnable;
  motorPinlookup[DOOR_ACTUATOR_COMM] = (void *)&ServoCtrl;
  
  sensorPinlookup[LINEAR_POT_COMM] = (void *)&LinearPot;
  sensorPinlookup[MAGNETIC_1_COMM] = (void *)&Pullup1;
  sensorPinlookup[MAGNETIC_2_COMM] = (void *)&Pullup2;
  sensorPinlookup[WEIGHT_COMM] = (void *)&MiscSensor;
  sensorPinlookup[DRUMENC_1_COMM] = (void *)&BlueMotor;
  sensorPinlookup[DRUMENC_2_COMM] = (void *)&GoldMotor;
  sensorPinlookup[TOP_LIMIT_COMM] = (void *)&Pullup4;
  sensorPinlookup[BOTTOM_LIMIT_COMM] = (void *)&Pullup5;
}
