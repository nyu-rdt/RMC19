

int incomingByte[256];
//int d = 10000;
#define a 1000;
int i =0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void readPhase(){
//  while(Serial1.available()>0 || count != 0){
//    incomingByte = Serial1.read();
//    count = d-a;
  //}
  int count = a;
  while(count!=0){
    if(Serial1.available()>0){
      incomingByte[i] = Serial1.read();
      i++;
      count--;
      if(i == 256){
        break;
      }
    }
//    else{
//      delay(d);
// 
//    }
  } 
}
