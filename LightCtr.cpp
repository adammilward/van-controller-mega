/*
 * LightCtr.cpp
 *
 *  Created on: 22 Feb 2017
 *      Author: Adam Milward
 */

//#define DEBUG
#include "LightCtr.h"

LightCtr::controllerMode LightCtr::ctrMode = STATIC;
uint16_t LightCtr::fadeDelay = 0;
uint32_t LightCtr::waitMillisFade = 0;
uint8_t LightCtr::slideDelay = 100;
uint32_t LightCtr::waitMillisSlide = 0;

const char LightCtr::oneWordCommands
				[LightCtr::oneWordCount]
				 [LightCtr::oneWordLength]
				  PROGMEM = {
					{"report"},
					{"on"},
					{"off"},
					{"static"},
					{"lin"},
					{"sin"},
					{"exp"},
					{"sinexp"},
					{"ru"},
					{"rd"},
					{"bu"},
					{"bd"},
					{"gu"},
					{"gd"}
				};		

const char LightCtr::wordAndFloatCommands
				[LightCtr::wordAndFloatCount]
				 [LightCtr::wordAndFloatLength]
				  PROGMEM = {
					{"report"},
					{"r"},
					{"g"},
					{"b"},
					{"delay"},
					{"u"},
					{"l"}
				};


LightCtr::LightCtr() {}

bool LightCtr::actionSerial(char **wordPtrs, byte wordCount){
	if (0 == wordCount) {
		return true;
	} else if (1 == wordCount) {
        if (actionOneWord(wordPtrs)) {
        	report();
        	return true;
        }
    } else if (2 == wordCount) {
        if (parseTwoWords(wordPtrs)) {
        	report();
        	return true;
        }
    }
	help();
    return false;
}

void LightCtr::help() {
	Gbl::strPtr->println(F("<{'mode': 'lights', 'err': 'command not recognised'}>"));
	Gbl::strPtr->println(F("Command not recognised, options are:"));
	Gbl::strPtr->println(F("Command not recognised, options are:"));
    Gbl::strPtr->println(F("- Single Word Commands "));
    int i = oneWordCount;
    while (i--) {
		char oneItem[oneWordLength];
		memcpy_P (&oneItem, &oneWordCommands[i], sizeof oneItem);
		Gbl::strPtr->println(oneItem);
    }
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
    Gbl::strPtr->println(F("-   word + number "));
    i = wordAndFloatCount;
    while (i--) {
		char oneItem[wordAndFloatLength];
		memcpy_P (&oneItem, &wordAndFloatCommands[i], sizeof oneItem);
    	Gbl::strPtr->print(oneItem);
    	Gbl::strPtr->println(F(" [nn]"));
    }
    Gbl::strPtr->println(F("eg. 'report 10';"));
}

bool LightCtr::actionOneWord(char **wordPtrs){
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionOneWord"));
	Gbl::freeRam();
	Gbl::strPtr->println(F("*wordPtrs"));
	Gbl::strPtr->println(*wordPtrs);
#endif
	int i = oneWordCount;
    while (i--) {
		// make a copy of the current one
		char oneItem[oneWordLength];
		memcpy_P (&oneItem, &oneWordCommands[i], sizeof oneItem);
        if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*oneWordActions[i])();
            return true;
        }
    }
	return false;
}

bool LightCtr::parseTwoWords(char **wordPtrs) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::parseTwoWods"));
	Gbl::freeRam();
#endif
	if (Gbl::isNum(wordPtrs[1])) {
        return actionWordAndFloat(wordPtrs, atof(wordPtrs[1]));
    } else {
        Gbl::strPtr->println(F("Err: second word must be float"));
    }
}

bool LightCtr::actionWordAndFloat(char **wordPtrs, float value) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionWordAndFloat"));
	Gbl::freeRam();
#endif
	int i = wordAndFloatCount;
    while (i--) {
    	char oneItem[wordAndFloatLength];
		memcpy_P (&oneItem, &wordAndFloatCommands[i], sizeof oneItem);
        if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*wordAndFloatActions[i])(value);
            return true;
        }
    }
    return false;
}

void LightCtr::stopReport() {
	setReportDelay(0);
}

void LightCtr::setReportDelay(float delaySeconds) {
    reportDelaySec = (byte)delaySeconds;
}

void LightCtr::timer(unsigned long millis) {
	if (reportDelaySec
			&&
			(unsigned long)(millis - waitMillisReport)
				>= (unsigned long)reportDelaySec * 1000)
	   {
		   waitMillisReport = millis;
		   report();
	   }
	if (slideDelay
			&& (unsigned long)(millis - waitMillisSlide) >= slideDelay)
	{
		interrupt();
		waitMillisSlide = millis;
	}
	if (fadeDelay
			&&
			(unsigned long)(millis - waitMillisFade) >= fadeDelay)
	{
		onOffFade();
		waitMillisFade = millis;
	}
}

void LightCtr::setFadeSlow(byte timerMins, int8_t inShiftOp) {
	red->shiftOp = inShiftOp;
	green->shiftOp = inShiftOp;
	blue->shiftOp = inShiftOp;
	red->gain = 0.02;
	green->gain = 0.02;
	blue->gain= 0.02;

	// steps = DeltaBase * No.Lights / Gain  =  2 * 3 / 0.02 = 300
	// use 270 steps so it has chance to reach full power
	int steps = 270;
	if (-1 == inShiftOp) {
		float gain = red->base + blue->base + green->base;
		steps = gain * 1.2 / 0.02; // todo assess timing
	}
	fadeDelay = (uint32_t)timerMins * 1000 * 60 / steps;
	slideDelay = 0;
	Light::fMode = Light::EXP;
}

void LightCtr::setFadeOffQuick(byte delayMillis) {
	red->shiftOp = -1;
	green->shiftOp = -1;
	blue->shiftOp = -1;
	red->gain = 0.2;
	green->gain = 0.2;
	blue->gain= 0.2;

	fadeDelay = (uint32_t)delayMillis * 100;
	slideDelay = 0;
	Light::fMode = Light::EXP;
}

void LightCtr::report() {
	Gbl::strPtr->println(F("<{"));
	Gbl::strPtr->println(F(" 'mode': 'lights'"));
	Gbl::strPtr->print(F(",'r': "));
    Gbl::strPtr->println(red->power);
	Gbl::strPtr->print(F(",'g': "));
    Gbl::strPtr->println(green->power);
	Gbl::strPtr->print(F(",'b': "));
    Gbl::strPtr->println(blue->power);

	Gbl::strPtr->print(F(",'l': "));
    Gbl::strPtr->println(red->lower);
	Gbl::strPtr->print(F(",'u': "));
    Gbl::strPtr->println(( red->lower + red->range * 255));
	Gbl::strPtr->print(F(",'range': "));
    Gbl::strPtr->println(red->range * 255);

	Gbl::strPtr->print(F(",'lightMode': "));
	Gbl::strPtr->print(F("["));
	Gbl::strPtr->print(ctrMode);
	Gbl::strPtr->print(F(","));
	Gbl::strPtr->print(Light::fMode);
	Gbl::strPtr->println(F("]"));

	Gbl::strPtr->print(F(",'delay': "));
	Gbl::strPtr->println(slideDelay);
	Gbl::strPtr->print(F(",'fadeDelay': "));
	Gbl::strPtr->println(fadeDelay);
    Gbl::strPtr->print(F(",'reportDelay': "));
    Gbl::strPtr->println(reportDelaySec * 1000);
	Gbl::strPtr->println(F("}>"));

	// Serial.println("range");
	// Serial.println(red->range);
	// Serial.println(red->range * 255);
	// Serial.println(red->lower);
	// Serial.println(red->lower + red->range * 255);
	
	

	// Gbl::strPtr->print(F("Red      base: "));
    // Gbl::strPtr->print(red->base * 100);
    // Gbl::strPtr->print(F("  power: "));
    // Gbl::strPtr->println(red->power);

    // Gbl::strPtr->print(F("Green  base: "));
    // Gbl::strPtr->print(green->base * 100);
    // Gbl::strPtr->print(F("  power: "));
    // Gbl::strPtr->println(green->power);

    // Gbl::strPtr->print(F("Blue     base: "));
    // Gbl::strPtr->print(blue->base * 100);
    // Gbl::strPtr->print(F("  power: "));
    // Gbl::strPtr->println(blue->power);

    // Gbl::strPtr->print(F("Red      gain: "));
    // Gbl::strPtr->println(red->gain * 1000);

    // Gbl::strPtr->print(F("Green  gain: "));
    // Gbl::strPtr->println(green->gain * 1000);

    // Gbl::strPtr->print(F("Blue     gain: "));
    // Gbl::strPtr->println(blue->gain * 1000);

    // Gbl::strPtr->print(F("Red      lower: "));
    // Gbl::strPtr->print(red->lower);
    // Gbl::strPtr->print(F("  range: "));
    // Gbl::strPtr->println(red->range);

    // Gbl::strPtr->print(F("Green  lower: "));
    // Gbl::strPtr->print(green->lower);
    // Gbl::strPtr->print(F("  range: "));
    // Gbl::strPtr->println(green->range);

    // Gbl::strPtr->print(F("Blue     lower: "));
    // Gbl::strPtr->print(blue->lower);
    // Gbl::strPtr->print(F("  range: "));
    // Gbl::strPtr->println(blue->range);


	// Gbl::strPtr->print(F("ctrMode: "));
	// Gbl::strPtr->println(ctrMode);
	// Gbl::strPtr->print(F("FadeMode: "));
	// Gbl::strPtr->println(Light::fMode);

	// Gbl::strPtr->print(F("slideDelay (millis): "));
	// Gbl::strPtr->println(slideDelay);
	// Gbl::strPtr->print(F("fadeDelay (millis): "));
	// Gbl::strPtr->println(fadeDelay);

    // Gbl::strPtr->print(F("report delay seconds = "));
    // Gbl::strPtr->println(reportDelaySec);
}

void LightCtr::interrupt(){
    static int counter = 0;  // increments each repetition
    switch (counter) {
    case 1:
        // slide should not be more than 0.002 for smootheness
        red->slide();
        break;
    case 2:
        green->slide();
        break;
    default:
        blue->slide();
        counter = 0;
        break;
    }
    counter++;
}

void LightCtr::onOffFade(){
    static int counter = 0;  // increments each repetition
    if (red->shiftOp == 1) {
    	// fading ON
    	switch (counter) {
		case 1:
			red->fade();
			break;
		case 2:
			if (red->base > 0.5 ) green->fade();
			break;
		default:
			if (red->base >= 1) {
				blue->fade();
				if (blue->base >= 1) {
					// can only be reached last of all
					turnFadingOff();
				}
			}
			counter = 0;
			break;
		}
    } else {
    	//fading off, could have them all fade together
    	switch (counter) {
    		case 1:
    			blue->fade();
    			break;
    		case 2:
    			if (blue->base < 0.5) green->fade();
    			break;
    		default:
    			if (blue->base <= 0) {
    				red->fade();
					if (red->base <= 0) {
						red->set(-1);
						green->set(-1);
						blue->set(-1);
						turnFadingOff();
					}
    			}
    			counter = 0;
    			break;
    		}
    }
    counter++;

	#ifdef DEBUG
		Serial.print(counter);
		Serial.print("  ");
		Serial.print(red->base);
		Serial.print("  ");
		Serial.print(green->base);
		Serial.print("  ");
		Serial.print(blue->base);
		Serial.println();
		//report();
	#endif
}

void LightCtr::redSet(float inBase) {
    red->set((inBase * 0.0101) - 0.01);
}
void LightCtr::greenSet(float inBase) {
    green->set((inBase * 0.0101) - 0.01);
}
void LightCtr::blueSet(float inBase) {
    blue->set((inBase * 0.0101) - 0.01);
}

void LightCtr::allTop(){
    red->set(1);
    green->set(0.5);
    blue->set(0.5);
}

void LightCtr::off (){ //off
	setFadeOffQuick(1);
}

void LightCtr::redUp   () { red->shift(+1);}
void LightCtr::greenUp () { green->shift(+1); }
void LightCtr::blueUp () { blue->shift(+1); }
void LightCtr::redDown() { red->shift(-1); }
void LightCtr::greenDown() { green->shift(-1); }
void LightCtr::blueDown() { blue->shift(-1); }


void LightCtr::lin    () {
	turnSlideOn();
    Light::fMode = Light::LIN;
	Gbl::strPtr->println(F("fadeMode LIN"));
}
void LightCtr::sin    () {
	turnSlideOn();
    Light::fMode = Light::SIN;
	Gbl::strPtr->println(F("fadeMode SIN"));
}
void LightCtr::exp    () {
	turnSlideOn();
    Light::fMode = Light::EXP;
	Gbl::strPtr->println(F("fadeMode EXP"));
}
void LightCtr::sinExp    () {
	turnSlideOn();
    Light::fMode = Light::EXPSIN;
	Gbl::strPtr->println(F("fadeMode EXPSIN"));
}


void LightCtr::turnFadingOff() {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::turnFadingOff"));
#endif
	ctrMode = STATIC;
    fadeDelay = 0;
    slideDelay = 0;
}

void LightCtr::turnSlideOn(){
	#ifdef DEBUG
		Gbl::strPtr->println(F("LightCtr::turnSlideOn"));
	#endif
	ctrMode = FADE;
	if (! slideDelay) slideDelay = 100;
	fadeDelay = 0;
}

void LightCtr::delaySet(float inDelay) {
	slideDelay = (inDelay < Gbl::DELAY_MIN) ? Gbl::DELAY_MIN :
		(inDelay > Gbl::DELAY_MAX) ? Gbl::DELAY_MAX :
			inDelay;
    turnSlideOn();
}

 void LightCtr::upperSet(float upper) {
     red->setUpper(upper);
     green->setUpper(upper);
     blue->setUpper(upper);
}

void LightCtr::lowerSet(float lower) {
    red->setLower(lower);
    green->setLower(lower);
    blue->setLower(lower);
}