#ifndef __INDICATOR_H
#define __INDICATOR_H

#include <Arduino.h>
class Indicator{
public:
  Indicator(){
    init();
  }
  void init(){
    pinMode(forwardPort, OUTPUT);
    digitalWrite(forwardPort, LOW);
    pinMode(backwardPort, OUTPUT);
    digitalWrite(backwardPort, LOW);
    pinMode(leftPort, OUTPUT);
    digitalWrite(leftPort, LOW);
    pinMode(rightPort, OUTPUT);
    digitalWrite(rightPort, LOW);
    pinMode(connectionBreakPort, OUTPUT);
    digitalWrite(connectionBreakPort, LOW);
    pinMode(digPort, OUTPUT);
    digitalWrite(digPort, LOW);
    pinMode(dumpingPort, OUTPUT);
    digitalWrite(dumpingPort, LOW);
  }
  void refreshIndicator(){
    digitalWrite(forwardPort, forward ? HIGH : LOW);
    digitalWrite(backwardPort, backward ? HIGH : LOW);
    digitalWrite(leftPort, left ? HIGH : LOW);
    digitalWrite(rightPort, right ? HIGH : LOW);
    digitalWrite(connectionBreakPort, connectionBreak ? HIGH : LOW);
    digitalWrite(digPort, dig ? HIGH : LOW);
    digitalWrite(dumpingPort, dumping ? HIGH : LOW);
  }
  void setForward(){forward = 1;}
  void setBackward(){backward = 1;}
  void setLeft(){left = 1;}
  void setRight(){right = 1;}
  void setConnectionBreak(){connectionBreak = 1;}
  void setDig(){dig = 1;}
  void setDumping(){dumping = 1;}
  void resetForward(){forward = 0;}
  void resetBackward(){backward = 0;}
  void resetLeft(){left = 0;}
  void resetRight(){right = 0;}
  void resetConnectionBreak(){connectionBreak = 0;}
  void resetDig(){dig = 0;}
  void resetDumping(){dumping = 0;}
  unsigned char forwardPort = 22;
  unsigned char backwardPort = 23;
  unsigned char leftPort = 24;
  unsigned char rightPort = 25;
  unsigned char connectionBreakPort = 26;
  unsigned char digPort = 27;
  unsigned char dumpingPort = 28;
private:
  unsigned char forward = 0;
  unsigned char backward = 0;
  unsigned char left = 0;
  unsigned char right = 0;
  unsigned char connectionBreak = 0;
  unsigned char dig = 0;
  unsigned char dumping = 0;
};


#endif
