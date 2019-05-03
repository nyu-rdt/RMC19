#ifndef __LIMIT_SWITCH_H
#define __LIMIT_SWITCH_H

#include <Arduino.h>

class LimitSwitch{
public:
	LimitSwitch(){
		init();
	}
	void init(){
		pinMode(ls1Port, INPUT_PULLUP);
		pinMode(ls2Port, INPUT_PULLUP);
	}
	unsigned char checkLimits(){
		ls1 = getLS1();
		ls2 = getLS2();
		if(ls1 == HIGH && ls2 == HIGH)
			return 0;
		else if(ls1 == HIGH && ls2 == LOW)
			return 1;
		else if(ls1 == LOW && ls2 == HIGH)
			return 2;
		else if(ls1 == LOW && ls2 == LOW)
			return 3;
	}
	unsigned char getLS1(){return digitalRead(ls1Port);}
	unsigned char getLS2(){return digitalRead(ls2Port);}
	unsigned char ls1Port = 29;
	unsigned char ls2Port = 30;
private:
	unsigned ls1 = 0;
	unsigned ls2 = 0;
};

#endif
