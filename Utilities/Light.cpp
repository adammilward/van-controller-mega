
/*
 * Light.cpp
 *
 *  Created on: 5 Dec 2016
 *      Author: Adam Milward
 */


#include "Light.h"


Light::fadeMode Light::fMode = Light::SIN;

Light::Light(
        byte inPin,
        byte ID,
		float inGain,
		float inLower,
		float inUpper)
{
    pin = inPin;            // sets the pin
    id = ID;
    gain = (inGain == 0)? randomize(): inGain;
	range = inUpper - inLower; // range between 0 and 1
	lower = inLower * 254 + 1;
	base = inUpper;
	set(0.2);
}


// change change power by a given gain or default
// int op = -1 or 1
// float shiftGain < 1
void Light::shift(char op, float shiftGain) {
#ifdef MYBUG
    Serial.print(F("op"));
    Serial.println(op);
    Serial.print(F("shiftGain"));
    Serial.println(shiftGain);
#endif
	float setBase = base + op * shiftGain;
	set(setBase, true);
}

//  called after a shift is made to the base, or directly to set brightness
// float setBase <=1 (0 is lowest possible, < 0 is off);
// bool flash, true if you want to flash for extremes (0 or 1)
void Light::set(float setBase, bool flash) {
    //Serial.print("setBase");
    //Serial.println(setBase);
    // default is -2 and lights are set from what ever the base is set to
	if (setBase > -2) {
		base = setBase;
	}
    //Serial.print("base");
    //Serial.println(base);

	float exponant;

	if (setBase < -0.001) {
		base = -0.001;
		if (flash == true) {
			analogWrite(pin, (64)); //flash to eighth brightness
			delay(20);              // delay for flash
		}
		power = 0;
		//Serial.println(F("LOW"));
		digitalWrite(pin, LOW);         //Set digital high
	} else if (setBase > 1) {
		base = 1;
		if (flash == true) {
			analogWrite(pin, (128));    //flash to half brightness
			delay(20);                  // delay for flash
		}
		power = 255;
		//Serial.println(F("HIGH"));
		digitalWrite(pin, HIGH);           //Set digital high
	} else {
	    base = setBase;
		exponant = 8 * base;
		power = int(pow(2, exponant));
		//Serial.print("set base to ");
		//Serial.println(setBase);
		analogWrite(pin, (power));
	}
	 Gbl::strPtr->print(F("base= "));
     Gbl::strPtr->println(base);
	 Gbl::strPtr->print(F("power= "));
     Gbl::strPtr->println(power);
}

// change power of light automatically
// using built in data members shiftOp and gain
void Light::slide() {
	base = base + (shiftOp * gain);	    // update base

	// switch direction if required and return value between 1 and 255
	if (base >= 1) {
		shiftOp = -1;	        // apply direction change
		base = 1;		        // set base to max
	} else if (base <= 0) {
		shiftOp = 1;
		base = 0;
		gain = randomize();
		//Serial.println(gain*10000);
	} else {
	}
	calcPow();	            // calculate led power 1 to 255
	analogWrite(pin, (power));
}
// for fading lights, called by Slide()
void Light::calcPow() {
	float temp; // declare the temporary float for calculations
	temp = base;

	switch (fMode) {
	case LIN:
		temp = temp * 254;	        // calculate led power 1 to 255
		break;
	case SIN:
		temp = 1 - temp;                 // invert the base ( 0 -> 1, 1 -> 0)
		temp = cos(temp * 3.14159265);	// calculate the sine wave -1 to +1
		temp = temp + 1;            // temp is sinusoidal from 0 to 2
		temp = temp * 127;	            // modify led power 1 to 255
		break;
	case EXP:
		temp = 7.994353437 * temp;	    // set exponant 0 to 8ish
		temp = pow(2, temp);	    // power = 2^temp
		break;
	case EXPSIN:
		temp = 1 - temp;               // invert the base ( 0 -> 1, 1 -> 0)
		temp = cos(temp * 3.14159265) + 1; 	// calculate the sine wave 0 to 2
		temp = temp * 7.994353437 / 2 ;      // calculate exponant ( 0 to 8ish)
		temp = int(pow(2, temp));	// power = 2^temp
		break;
	default:
		break;
	}
	power = temp * range + lower;
/*    Serial.print(power);
    Serial.print("    ");
    Serial.print(temp);
    Serial.print("    ");
    Serial.print(range);
    Serial.print("    ");
    Serial.print(lower);
    Serial.println("    ");*/

	if (id != 0) {
	    power = power * 0.5 + 0.5;
	}

//	Serial.print("fMode   ");
//	Serial.print(fMode);
//	Serial.print(" base   ");
//	Serial.print(base);
//	Serial.print(" power   ");
//	Serial.println(power);
//	Serial.println(gain*1000);
}
float Light::randomize() {
    // 0.001 to 0.008
    return random(2, 20)/10000.0;
}
// called when resetting fade
// sets the base to half, calculates the power of lights
// and writes to pin
void Light::toHalf() {
    base = 0.5;
    calcPow();
    analogWrite(pin, (power));
}
void Light::flashOff(){
    digitalWrite(pin, LOW);
    delay(20);
    analogWrite(pin, power);
}
void Light::flashHalf(){
    analogWrite(pin, 64);
    delay(20);
    analogWrite(pin, power);
}
void Light::flashOn(){
    digitalWrite(pin, HIGH);
    delay(20);
    analogWrite(pin, power);
}
void Light::changeLower(char op, float change) {
    /*Serial.println();
    Serial.println();
    Serial.print("lower: ");
    Serial.println(lower);
    Serial.print("range: ");
    Serial.println(range);
    Serial.println();*/

    float tempLower = (lower - 1) /  254;
    float tempUpper = range + tempLower;

    tempLower = tempLower + (op * change);
    if (tempLower < 0 ) {
        tempLower = 0;
        flashHalf();
    } else if (tempLower > 0.6) {
        tempLower = 0.6;
        flashOn();
    }
    if (tempUpper - tempLower < 0.2) {
        tempUpper = tempLower + 0.2;
    }
    range = tempUpper - tempLower; // range between 0 and 1
    lower = tempLower * 254 + 1;
    base = 0;
    flashOff();

     Gbl::strPtr->print(F("upper= "));
     Gbl::strPtr->println(tempUpper);
     Gbl::strPtr->print(F("lower= "));
     Gbl::strPtr->println(tempLower);
}
/**
 * range from 0.2 to 1 increments of 0.2
 */
void Light::changeUpper(char op, float change) {
   /* Serial.println();
    Serial.println();
    Serial.print("lower: ");
    Serial.println(lower);
    Serial.print("range: ");
    Serial.println(range);
    Serial.println();*/

    float tempLower = (lower - 1) /  254;
    float tempUpper = range + tempLower;

    tempUpper = tempUpper + (op * change);

    if (tempUpper > 1 ) {
        tempUpper = 1;
        flashOn();
    } else if (tempUpper < 0.2) {
        tempUpper = 0.2;
    }
    if (tempUpper - tempLower < 0.2) {
        tempLower = tempUpper - 0.2;
        flashHalf();
    }
    range = tempUpper - tempLower; // range between 0 and 1
    lower = tempLower * 254 + 1;
    base = 1;
    flashOff();

     Gbl::strPtr->print(F("upper= "));
     Gbl::strPtr->println((tempUpper));
     Gbl::strPtr->print(F("lower= "));
     Gbl::strPtr->println((tempLower));
}


