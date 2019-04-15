// DRUM DEFINITIONS
#define DRUM1 1
#define DRUM2 2
#define LINEAR_ACTUATOR_1 4
#define LINEAR_ACTUATOR_2 8
#define VIBRATION_MOTORS 16
#define DOOR_LINEAR_ACTUATOR 32

#define LINEAR_POT 1
#define MAGNETIC_1 2
#define MAGNETIC_2 4
#define WEIGHT     8
#define DRUMENC_1  16
#define DRUMENC_2  32
#define TOP_LIMIT  64
#define BOTTOM_LIMIT 128

#define start_byte_motor 240
#define start_byte_sensor 241
#define stop_byte 255

#define MOTOR_TYPE 1
#define SENSOR_TYPE 2
#define VALUE_TYPE 3
#define STOP_TYPE 4
#define START_TYPE 5

int drum1 = 0,drum2 = 0,linear_actuator_1 = 0,linear_actuator_2 = 0,vibration_motors = 0,door_linear_actuators = 0;
int linear_pot, magnetic_1, magnetic_2,weight,drumenc_1, drumenc_2, top_limit, bottom_limit;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}


void parsePhase(){
  int next_byte_type = 0;
  int next_byte_motor = 0;
  int next_byte_sensor = 0;
  int motor_num = 0;
  int sensor_num = 0;
  for(int a=0;a<end_of_buffer;a++)
  {
    if(next_byte_type == START_TYPE && incomingByte[a]==start_byte_motor )
    {
      next_byte_type = MOTOR_TYPE;//1 means motor
      next_byte_motor = incomingByte[a+1];
    }
    else if(next_byte_type == START_TYPE && incomingByte[a] == start_byte_sensor){
      //means sensor
      next_byte_type = SENSOR_TYPE;
      next_byte_sensor = incomingByte[a+1];  
    }
    else if(next_byte_type == MOTOR_TYPE){
      motor_num = incomingByte[a];
      next_byte_type = VALUE_TYPE;
    }
    else if (next_byte_type == VALUE_TYPE) {
      switch(motor_num)
      {
        case DRUM1:
          drum1 = incomingByte[a];
          break;
        case DRUM2:
          drum2 = incomingByte[a];
          break;       
        case LINEAR_ACTUATOR_1:
          linear_actuator_1 = incomingByte[a];
          break;
        case LINEAR_ACTUATOR_2:
          linear_actuator_2 = incomingByte[a];
          break;
        case VIBRATION_MOTORS:
          vibration_motors = incomingByte[a];
          break;
        case DOOR_LINEAR_ACTUATOR:
          door_linear_actuator = incomingByte[a];
          break;
      }
      next_byte_type = STOP_TYPE;
    }
    else if(next_byte_type == SENSOR_TYPE){
      switch(incomingByte[a])
      {
        case LINEAR_POT:
          linear_pot = 1;
          break;
        case MAGNETIC_1:
          magnetic_1 = 1;
          break;
        case MAGNETIC_2:
          magnetic_2 = 1;
          break;
        case WEIGHT:
          weight = 1;
          break;
        case DRUMENC_1:
          drumenc_1 = 1;
          break;
        case DRUMENC_2:
          drumenc_2 =1;
          break;
        case TOP_LIMIT:
          top_limit =1;
          break;
        case BOTTOM_LIMIT:
          bottom_limit 1;
          break;
      }
      next_byte_type = STOP_TYPE;
    }
    else if (next_byte_type == STOP_TYPE && incomingByte[a] == stop_byte){
      next_byte_type = START_TYPE;
      continue;
    }
  }

  if (next_byte_type == START_TYPE) {
    end_of_buffer = 0; // Clear the buffer
  }
  else {
    // read backwards until START_BYTE
    // copy data to beginning of buffer
    // move end_of_buffer
    int b = 0;
    for(a = end_of_buffer - 1; a >= 0; a--){
      if(incomingByte[a]==start_byte_motor || incomingByte[a] == start_byte_sensor){
        b = a;
        break;
      }
    }

    counter = 0;
    for (int c = b; c < end_of_buffer; c++){
      incomingByte[counter] = incomingByte[c];
      ++counter;
    }

    end_of_buffer = counter;
  }
}
