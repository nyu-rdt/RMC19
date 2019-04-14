#define RESPONSE 0xF2
#define STOP_BYTE 0xFF
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void execute(){
  if(linear_pot){
   Serial1.write(RESPONSE); // 0xF2
   Serial1.write(LINEAR_POT);
   Serial1.write(pot_linear_meas);
   Serial1.write(STOP_BYTE); // 0xFF

    linear_pot = 0;
  }
  if(magnetic_1){
    Serial1.write(RESPONSE); // 0xF2
    Serial1.write(MAGNETIC_1);
    Serial1.write(mag_1_meas);
    Serial1.write(STOP_BYTE);;
    magnetic_1 = 0;
  }
  if(magnetic_2){
    Serial1.write(RESPONSE); // 0xF2
    Serial1.write(MAGNETIC_2);
    Serial1.write(mag_2_meas);
    Serial1.write(STOP_BYTE); // 0xFF
    magnetic_2 = 0;
  }
  if(weight){
    Serial1.write(RESPONSE); // 0xF2
    Serial1.write(WEIGHT);
    Serial1.write(wt_meas);
    Serial1.write(STOP_BYTE); // 0xFF
    weight = 0;
  }
  if(top_limit){
    Serial1.write(RESPONSE); // 0xF2
    Serial1.write(TOP_LIMIT);
    Serial1.write(top_lmt_meas);
    Serial1.write(STOP_BYTE); // 0xFF
    top_limit = 0;
  }
  if(bottom_limit)
  {
    Serial1.write(RESPONSE); // 0xF2
    Serial1.write(BOTTOM_LIMIT);
    Serial1.write(bottom_lmt_meas);
    Serial1.write(STOP_BYTE); // 0xFF
    bottom_limit = 0;
  }
}
