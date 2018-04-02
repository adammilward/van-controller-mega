/*
 * LightCtr.cpp
 *
 *  Created on: 22 Feb 2017
 *      Author: Adam Milward
 */

//#define DEBUG

#include "LightCtr.h"
#include "Controller.h"
#include "Gbl.h"

LightCtr::controllerMode LightCtr::ctrMode = STATIC;
uint16_t LightCtr::fadeDelay = 0;
uint32_t LightCtr::waitMillisFade = 0;
uint8_t LightCtr::slideDelay = 100;
uint32_t LightCtr::waitMillisSlide = 0;

const char LightCtr::remoteAlias
				[LightCtr::remoteAliasCount]
				 [LightCtr::remoteAliasLength]
		  PROGMEM = {
		{"up"},   {"down"},   {"on"},   {"off"},
		{"red"},   {"green"},   {"blue"},   {"white"},
		{"orange"},   {"yellow"},   {"cyan"},   {"purple"},
		{"j1"},   {"j2"},   {"j3"},   {"j4"},
		{"m1"},  {"m2"},   {"m3"},   {"m4"}
};

const char LightCtr::oneWordCommands
				[LightCtr::oneWordCount]
				 [LightCtr::oneWordLength]
				  PROGMEM = {
					{"report"},
					{"top"},
					{"bottom"},
					{"static"},
					{"lin"},
					{"sin"},
					{"exp"},
					{"sinexp"}
};

const char LightCtr::firstOfTwoCommands
				[LightCtr::firstOfTwoCount]
				 [LightCtr::firstOfTwoLength]
				  PROGMEM = {
           "all", "red", "green", "blue",
           "lower", "upper", "delay"
};

const char LightCtr::secondOfTwoCommands
				[LightCtr::secondOfTwoCount]
				 [LightCtr::secondOfTwoLength]
				  PROGMEM = {
		{"up"}, {"down"}, {"bottom"}, {"top"}, {"off"}
};

const char LightCtr::wordAndFloatCommands
				[LightCtr::wordAndFloatCount]
				 [LightCtr::wordAndFloatLength]
				  PROGMEM = {
		{"report"},
		{"all"},
		{"red"},
		{"green"},
		{"blue"},
		{"delay"}
};


LightCtr::LightCtr() {}

bool LightCtr::actionSerial(char **wordPtrs, byte wordCount){
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionSerial"));
	Gbl::freeRam();
	Gbl::strPtr->println(F("wordCount"));
	Gbl::strPtr->println(wordCount);
#endif
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
    Gbl::strPtr->println(F("Command not recognised, options are:"));
    Gbl::strPtr->println(F("the name of a button on the remote"));
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
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
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
    Gbl::strPtr->println(F("- Two Word Commands"));
    Gbl::strPtr->println(F("all|red|green|blue|lower|upper|delay"));
    Gbl::strPtr->println(F("+"));
    Gbl::strPtr->println(F("up|down|top|bottom"));
    Gbl::strPtr->println(F("eg. delay top"));
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
    i = remoteAliasCount;
    while (i--) {
    	// make a copy of the current one
		char oneItem[remoteAliasLength];
		memcpy_P (&oneItem, &remoteAlias[i], sizeof oneItem);
    	if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*actions[ctrMode][i])();
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
	if (Controller::isNum(wordPtrs[1])) {
        return actionWordAndFloat(wordPtrs, atoi(wordPtrs[1]));
    } else {
        return actionTwoWords(wordPtrs);
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

bool LightCtr::actionTwoWords(char **wordPtrs) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionTwoWords"));
	Gbl::freeRam();
	Gbl::strPtr->println(wordPtrs[0]);
	Gbl::strPtr->println(wordPtrs[1]);
#endif
	// first word
	int i = firstOfTwoCount;
    while (i --) {
    	char oneItem[firstOfTwoLength];
    	memcpy_P (&oneItem, &firstOfTwoCommands[i], sizeof oneItem);
		//Gbl::strPtr->println(oneItem);
        if (strcasecmp(oneItem, wordPtrs[0]) == 0) {

        	//second word
        	int j = secondOfTwoCount;
            while (j--) {
            	char oneItem[secondOfTwoLength];
				memcpy_P (&oneItem, &secondOfTwoCommands[j], sizeof oneItem);
				//Gbl::strPtr->println(oneItem);
                if (strcasecmp(oneItem, wordPtrs[1]) == 0) {
                    (this->*twoWordActions[i][j])();
                    return true;
                }
            }
        }
    }
    return false;
}


bool LightCtr::actionRemote(unsigned long inValue){
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionRemote"));
#endif
    for (int i = 0; i < 20; i++) {
        if (codes[i] == inValue) {
            (this->*actions[ctrMode][i])();
            Gbl::strPtr->println(i);
            return true;
        }
    }
    return false;
}

void LightCtr::setReportDelay(float delaySeconds) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::setReportDelay"));
#endif
    reportDelaySec = (byte)delaySeconds;
    //waitMillisReport = millis(); // prob don't need this
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
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::setFadeOn"));
	Gbl::strPtr->print(F("timerMins: "));
	Gbl::strPtr->println(timerMins);
	Gbl::freeRam();
#endif
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
		steps = gain * 1.5 / 0.02;
	}
	fadeDelay = (uint32_t)timerMins * 1000 * 60 / steps;
	slideDelay = 0;
	Light::fMode = Light::EXP;
}

void LightCtr::setFadeOffQuick(byte delayMillis) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::setFadeOff"));
	Gbl::strPtr->print(F("timerMins: "));
	Gbl::strPtr->println(delayMillis);
	Gbl::freeRam();
#endif
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
    Gbl::strPtr->print(F("Red      base: "));
    Gbl::strPtr->print(red->base * 100);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(red->power);

    Gbl::strPtr->print(F("Green  base: "));
    Gbl::strPtr->print(green->base * 100);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(green->power);

    Gbl::strPtr->print(F("Blue     base: "));
    Gbl::strPtr->print(blue->base * 100);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(blue->power);

    Gbl::strPtr->print(F("Red      gain: "));
    Gbl::strPtr->println(red->gain * 1000);

    Gbl::strPtr->print(F("Green  gain: "));
    Gbl::strPtr->println(green->gain * 1000);

    Gbl::strPtr->print(F("Blue     gain: "));
    Gbl::strPtr->println(blue->gain * 1000);

    Gbl::strPtr->print(F("Red      lower: "));
    Gbl::strPtr->print(red->lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(red->range);

    Gbl::strPtr->print(F("Green  lower: "));
    Gbl::strPtr->print(green->lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(green->range);

    Gbl::strPtr->print(F("Blue     lower: "));
    Gbl::strPtr->print(blue->lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(blue->range);


	Gbl::strPtr->print(F("ctrMode: "));
	Gbl::strPtr->println(ctrMode);
	Gbl::strPtr->print(F("FadeMode: "));
	Gbl::strPtr->println(Light::fMode);

	Gbl::strPtr->print(F("slideDelay (millis): "));
	Gbl::strPtr->println(slideDelay);
	Gbl::strPtr->print(F("fadeDelay (millis): "));
	Gbl::strPtr->println(fadeDelay);

    Gbl::strPtr->print(F("report delay seconds = "));
    Gbl::strPtr->println(reportDelaySec);
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

void LightCtr::retrieveStore(colour inColour){
    //Serial.print("retrieveStore Colour: ");
    //Serial.println(inColour);
    tempStore[0] = red->base;
    tempStore[1] = green->base;
    tempStore[2] = blue->base;
    //Serial.print("Stored Values: ");
    //Serial.print(colourStore[inColour][0]);    //Serial.print(colourStore[inColour][1]);
    //Serial.println(colourStore[inColour][2]);
    red->set(colourStore[inColour][0]);
    green->set(colourStore[inColour][1]);
    blue->set(colourStore[inColour][2]);
    turnFadingOff();
    Gbl::strPtr->println(F("lights STATIC"));
}
void LightCtr::storeThis(colour inColour){
    //Serial.print("store New Colour ");
    //Serial.println(inColour);
    red->set(tempStore[0]);
    green->set(tempStore[1]);
    blue->set(tempStore[2]);
    colourStore[inColour][0] = tempStore[0];
    colourStore[inColour][1] = tempStore[1];
    colourStore[inColour][2] = tempStore[2];
    //Serial.print("Stored Values: ");
    //Serial.print(colourStore[inColour][0]);
    //Serial.print(colourStore[inColour][1]);
    //Serial.println(colourStore[inColour][2]);
}

void LightCtr::allSet(float inBase) {
    red->set((inBase * 0.11) - 0.1);
    green->set((inBase * 0.11) - 0.1);
    blue->set((inBase * 0.11) - 0.1);
}
void LightCtr::redSet(float inBase) {
    red->set((inBase * 0.11) - 0.1);
}
void LightCtr::greenSet(float inBase) {
    green->set((inBase * 0.11) - 0.1);
}
void LightCtr::blueSet(float inBase) {
    blue->set((inBase * 0.11) - 0.1);
}

void LightCtr::allBot(){
    setFadeOffQuick(1);
}
void LightCtr::allTop(){
    red->set(1);
    green->set(1);
    blue->set(1);
}
void LightCtr::redBot() { red->set(0);}
void LightCtr::redTop() { red->set(1);}
void LightCtr::redOff() { red->set(-1);}
void LightCtr::greenBot() { green->set(0);}
void LightCtr::greenTop() { green->set(1);}
void LightCtr::greenOff() { green->set(-1);}
void LightCtr::blueBot() { blue->set(0);}
void LightCtr::blueTop() { blue->set(1);}
void LightCtr::blueOff() { blue->set(-11);}
void LightCtr::lowerBot() {
    red->changeLower(-1, 1);
    green->changeLower(-1, 1);
    blue->changeLower(-1, 1);
}
void LightCtr::lowerTop() {
    red->changeLower(1, 1);
    green->changeLower(1, 1);
    blue->changeLower(1, 1);
}
void LightCtr::upperBot() {
    red->changeUpper(-1, 1);
    green->changeUpper(-1, 1);
    blue->changeUpper(-1, 1);
}
void LightCtr::upperTop() {
    red->changeUpper(1, 1);
    green->changeUpper(1, 1);
    blue->changeUpper(1, 1);
}
void LightCtr::delayBot(){
    slideDelay = Gbl::DELAY_MIN;
    turnSlideOn();
}
void LightCtr::delayTop() {
    slideDelay = Gbl::DELAY_MAX;
    turnSlideOn();
}
void LightCtr::delaySet(float inDelay) {
    slideDelay = (int)inDelay;
    turnSlideOn();
}

void LightCtr::allUp(){
    red->shift(+1);
    green->shift(+1);
    blue->shift(+1);
}

void LightCtr::allDown(){
    red->shift(-1);
    green->shift(-1);
    blue->shift(-1);
}
void LightCtr::on (){ //off
    red->set(0.5);
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
void LightCtr::store1 () {
    if (holdCount == 0) {
        retrieveStore(RED);
    } else if (holdCount == 4) {
        storeThis(RED);
    }
    turnFadingOff();
}
void LightCtr::store2 () {
    if (holdCount == 0) {
        retrieveStore(GREEN);
    } else if (holdCount == 4) {
        storeThis(GREEN);
    }
    turnFadingOff();
}
void LightCtr::store3 () {
    if (holdCount == 0) {
        retrieveStore(BLUE);
    } else if (holdCount == 4) {
        storeThis(BLUE);
    }
    turnFadingOff();
}
void LightCtr::store4 () {
    if (holdCount == 0) {
        retrieveStore(WHITE);
    } else if (holdCount == 4) {
        storeThis(WHITE);
    }
    turnFadingOff();
}
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

void LightCtr::red_f(){
    red->changeLower(+1, 0.2);
    green->changeLower(+1, 0.2);
    blue->changeLower(+1, 0.2);
}
void LightCtr::orange_f (){
    red->changeLower(-1, 0.2);
    green->changeLower(-1, 0.2);
    blue->changeLower(-1, 0.2);
}
void LightCtr::green_f (){
    red->changeUpper(+1, 0.2);
    green->changeUpper(+1, 0.2);
    blue->changeUpper(+1, 0.2);
}
void LightCtr::yellow_f(){
    red->changeUpper(-1, 0.2);
    green->changeUpper(-1, 0.2);
    blue->changeUpper(-1, 0.2);
}
void LightCtr::white_f(){
	if ((int)slideDelay*4 > Gbl::DELAY_MAX) {
		slideDelay = Gbl::DELAY_MAX;
		red->flashHalf();
		green->flashHalf();
		blue->flashHalf();
	} else {
		slideDelay *= 4;
	}
    turnSlideOn();
}
void LightCtr::purple_f(){
	if ((int)slideDelay / 4 < Gbl::DELAY_MIN) {
		slideDelay = Gbl::DELAY_MIN;
		red->flashHalf();
		green->flashHalf();
		blue->flashHalf();
	} else {

		slideDelay /= 4;
	}
    turnSlideOn();
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
void LightCtr::null(){};
