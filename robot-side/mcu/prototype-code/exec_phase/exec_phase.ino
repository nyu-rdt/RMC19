#define DRUM_1 4
#define DRUM_2 5
#define LINEAR_ACTUATOR 3
#define LINEAR_ACTUATOR_DOOR A9
#define VIBRATION_MOTOR A6


#define POT_LINEAR A0
#define MAG_1 A1
#define MAG_2 A2
#define WT A7
#define TOP_LMT A10
#define BOTTOM_LMT A11

int pot_linear_meas, mag_1_meas, mag_2_meas, wt_meas, top_lmt_meas, bottom_lmt_meas;


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * Purple Motor: D4: Drum 1
 * Gold Motor: D5: Drum 2
 * Green Motor: D3: Linear Actuator
 * Servo: A9: Door Linear Actuator
 * Vibration Motor: A6: Vibration motors
 * 
 * Linear Pot: A0
 * Magnetic 1: Pullup 1: A1
 * Magnetic 2: Pullup 2: A2
 * Weight: load: A7
 * Drum enc 1: serial (ignore)
 * Drum enc 2: serial (ignore)
 * Top limit: pull up 4: A10 
 * Bottom limit: pull up 5: A11 
 * 
 * (all pull ups are digital (1/0))
 */

//linear_pot, magnetic_1, magnetic_2,weight,drumenc_1, drumenc_2, top_limit, bottom_limit;
void execPhase(){
  unsigned int m1,m2,m3,m4,m5,m6;
  m1=(unsigned)drum1*205/255+204;
  analogWrite(DRUM_1,m1);
 
  m2=(unsigned)drum2*205/255+204;
  analogWrite(DRUM_2,m2);
  
  m3=(unsigned)linear_actuator_1*205/255+204;
  analogWrite(LINEAR_ACTUATOR,m3);
         
  m5=(unsigned)vibration_motors*205/255+204;
  analogWrite(VIBRATION_MOTOR,m5);
 
  m5=(unsigned)door_linear_actuator*205/255+204;
  analogWrite(LINEAR_ACTUATOR_DOOR,m6);

//  Serial1.write(RESPONSE); // 0xF2
//  Serial1.write(LINEAR_POT);
//  Serial1.write(pot_linear_meas);
//  Serial1.write(END); // 0xFF

  linear_pot = 0;

// int pot_linear, mag_1, mag_2, wt, top_lmt, bottom_lmt;
  if(linear_pot){
    pot_linear_meas = analogRead(POT_LINEAR);
    
  }
  if(magnetic_1){
    mag_1_meas = digitalRead(MAG_1);
    
  }
  if(magnetic_2){
    mag_2_meas = digitalRead(MAG_2);
    
  }
  if(weight){
    wt_meas = analogRead(WT);
    
  }
  if(top_limit){
    top_lmt_meas = digitalRead(TOP_LMT);
    
  }
  if(bottom_limit)
  {
    bottom_lmt_meas = digitalRead(BOTTOM_LMT);
   
  }
  // TODO: Drum Encoders too!
}
