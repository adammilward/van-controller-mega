
/*
 * Light.h
 *
 *  Created on: 5 Dec 2016
 *      Author: Adam Milward
 */

#include "Gbl.h"
#include "Arduino.h"

#ifndef Light_h_
#define Light_h_

class Light{
public:
	Light(
		byte inPin,
		byte id,
		float inGain = 0,
		float inLower = 0,
		float inUpper = 1
		);


	// mode for automatic fading
    enum fadeMode { LIN, SIN, EXP, EXPSIN };
    static fadeMode fMode;

	float base;     // the base value  base <1, for deriving power <0 is off
	int power;      // led Power 1 to 255 derived from base
    float gain;         // for fading the lights <= 0.002
    float lower;        // lower limit 1 to 255
    float range;        // range/multiplier 0 to

	void shift(char op, float shiftGain = DEF_GAIN);
	void set(float setBase, bool flash = false);
	void slide();
	void toHalf();
	void changeLower(char op, float change = 0.2);
	void changeUpper(char op, float change = 0.2);
	void flashOff();
	void flashHalf();
	void flashOn();

private:

	static constexpr float DEF_GAIN = 0.1;     // default gain for use with Shift()

	byte pin;            // the pin for this colour
	byte id;             // 0 = red 1 = green 2 = blue
	// below are only used for fading
	int shiftOp = 1;    // 1 or -1  controls the direction of shift

	float randomize();
	void calcPow();     // called from Slide()
};

#endif /* LIGHT_H_ */

