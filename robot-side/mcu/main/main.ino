// These are the definiions for the phase numbers
#define READ_PHASE_NO         0
#define PARSE_PHASE_NO        1
#define EXEC_PHASE_NO         2
#define WRITE_PHASE_NO        3

// Include drum protocol definitions
#include "drum.h"

// Include general communications protocol definitions
#include "communications.h"

// Constants for read phase (parameters)
#define READ_BUFFER_LEN       256
#define READ_TIMEOUT_MILLIS   100    

// constants for parse phase (define byte types)
#define MOTOR_TYPE            1
#define SENSOR_TYPE           2
#define VALUE_TYPE            3
#define STOP_TYPE             4
#define START_TYPE            5

// Globals
int oPhase = 0;

// Serial Definitions
#define ESP_BAUD             115200
#define ROBOTEQ_BAUD          115200

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

// ARDUINO FUNCTIONS
void setup() {
  // start serial ports
  ESP.begin(ESP_BAUD);
  BlueMotor.begin(ROBOTEQ_BAUD);
  GoldMotor.begin(ROBOTEQ_BAUD);

  // Initialize motorExecBuffer to all 256
  memset(motorExecBuffer, 256, MOTOR_COMM_WIDTH);
  // Initialize sensorExecBuffer to all 0
  memset(sensorExecBuffer, 0, SENSOR_COMM_WIDTH);
}

void loop() {
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
  int start = millis(); // when the phase started
  int current = millis(); // current time of the phase

  while (current - start < READ_TIMEOUT_MILLIS) {
    // If there is a character to read
    if (ESP.available() > 0) {
      // Write to end of the readBuffer
      readBuffer[readBufferEnd] = ESP.read();
      ++readBufferEnd;
      current = millis();

      // Exit if the readbuffer is full
      if (readBufferEnd > READ_BUFFER_LEN) {
        break;
      }
    }
  } 
}

// This phase will take the received commands from the ESP and extracts the commands
// for each motor
void parsePhase() {
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
      sensorExecBuffer[readBuffer[a]] = 1;
      nextByteType = STOP_TYPE;
    }
    else if (next_byte_type == VALUE_TYPE) {
      // Set motor speed
      motorExecBuffer[motorIdentifier] = readBuffer[a];
      next_byte_type = STOP_TYPE;
    }
    else if (next_byte_type == STOP_TYPE && readBuffer[a] == stop_byte){
      next_byte_type = START_TYPE;
    }
  }

  // Check for incomplete segments
  if (next_byte_type == START_TYPE) { // The last byte received was STOP_BYTE
    readBufferEnd = 0; // Clear the buffer
  }
  else {
    // read backwards until START_BYTE
    for(a = readBufferEnd - 1; a >= 0; a--) {
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
  unsigned int scaled;
  
  for (int i = 1; i < MOTOR_COMM_WIDTH; i = i << 1) {
    scaled = (unsigned)motorExecBuffer[i] * 205 / 255 + 204;
    
    if (motorPinlookup[i] == BlueMotor || motorPinlookup[i] == GoldMotor) {
      // This is a SerialPort, gotta do something with that later
      continue;
    }
    analogWrite(motorPinlookup[i], scaled);
  }

  for (int i = 1; i < SENSOR_COMM_WIDTH; i = i << 1) {
    if (sensorExecBuffer[i]) {
      if (sensorPinlookup[i] > A0) { // Analog pin
        sensorValueBuffer[i] = analogRead(sensorPinlookup[i]);
      }
      else if (sensorPinlookup[i] == GoldMotor || sensorPinlookup[i] == BlueMotor) {
        // Serial port, gotta do this
      }
      else {
        sensorValueBuffer[i] = digitalRead(sensorPinlookup[i]);
      }
    }
  }
  // TODO: Drum Encoders too!
}
