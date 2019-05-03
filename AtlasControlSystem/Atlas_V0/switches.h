#ifndef __SWITCHES_H
#define __SWITCHES_H

#include <Arduino.h>

class Switches{
public:
	Switches(){
		init();
	}
	void init(){
		int i;
		for(i = 0; i < 8; i++){
			pinMode(swPort[i], OUTPUT);
			digitalWrite(swPort[i], 0);
		}

	}
	void setSwitch(unsigned char channel, unsigned char value){
		sw[channel] = value;
		digitalWrite(swPort[channel], value);
	}
	unsigned char getSwitch(unsigned char channel){
		return sw[channel];
	}
	void setAllSwitch(unsigned char value){
		int i;
		for(i = 0; i < 8; i++){
			if((value >> i) & 0x01){
				setSwitch(i, 1);
			}
			else{
				setSwitch(i, 0);
			}
		}
	}
	unsigned char getAllSwitch(){
		int i;
		int result = 0;
		for(i = 0; i < 8; i++){
			result |= sw[i] << i;
		}
		return result;
	}
	void setSingleSwitch(unsigned char value){
		int channel = value >> 5;
		int val = value & 0x01;
		setSwitch(channel, val);
	}
	void getSingleSwitch(unsigned char value){
		int channel = value >> 5;
		getSwitch(channel);
	}
private:
	unsigned char swPort[8] = {31,32,33,34,35,36,37,38};
	unsigned char sw[8] = {0};
};

#endif
