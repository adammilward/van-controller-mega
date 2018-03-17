
/*
 * RemoteReceive.cpp
 *
 *  Created on: 20 Feb 2017
 *      Author: Adam Milward
 */
#define DEBUG
//#define STAT
//#include "StatusCtr.h"
//#define LTS
//#define IR
//#include "LightCtr.h"
#define TME

#include "Arduino.h"
#include "Controller.h"
#include "Gbl.h"
#include "TimeCtr.h"


Controller::Controller() {
    // stop it going wappy first time something is sent
    prevDataAr[0] = '\0';
#ifdef STAT
   StatusCtr statusCtr;
#endif
#ifdef LTS
   LightCtr lightCtr;
#endif
#ifdef TME
   TimeCtr timeCtr;
#endif
}

void Controller::timer(unsigned long millis) {
#ifdef STAT
   statusCtr.timer(millis);
#endif
#ifdef LTS
    lightCtr.timer(millis);
#endif
#ifdef TME
    timeCtr.actionTimer(millis);
#endif
}

void Controller::serialReceive() {
#ifdef DEBUG
	Gbl::strPtr->println(F("serialRecieve"));
	Gbl::freeRam();
#endif
	serialRead();

	// if mode is set then start at the second word
	if (checkForMode()) {
		processSerial(1);
	} else {
		if (!processSerial(0)) {
			// if no mode and process failed
			Gbl::strPtr->println(F("I'm sorry Dave, I'm afraid I can't do that."));
			Gbl::strPtr->println(F("Modes are: 'lights', 'status', 'com' or 'clock'"));
			outputMode();
		}
	}
}

void Controller::serialRead() {
#ifdef DEBUG
	Gbl::strPtr->println(F("serialRead"));
	Gbl::freeRam();
#endif
	// put the data into the temporary array
	char dataAr[100];
	byte length = Gbl::strPtr->readBytesUntil(
		'\n',
		dataAr,
		sizeof(dataAr)/sizeof(char)
	);

	if (!checkForRepeat(dataAr)) {
		dataAr[length-1] = '\0'; // add null terminator to string
		bool spaceFlag = true;
		wordCount = 0;
		for(byte i = 0; i < length; i++ ) {
			// loop, set all spaces to null terminator recording into data member
			prevDataAr[i] = dataAr[i];
			// record the position of each word
			if (prevDataAr[i] == ' ' || wordCount == maxWords+1) {
				prevDataAr[i] = '\0';
				spaceFlag = true;
			} else if (spaceFlag == true) {
				wordPtrs[wordCount] = prevDataAr+i;
				spaceFlag = false;
				wordCount++;
			}
		}
		// can have no more than 5 words
		if (wordCount > maxWords) wordCount = maxWords;
	}
#ifdef DEBUG
		Gbl::strPtr->println(F("wordCoutn"));
		Gbl::strPtr->println(wordCount);
		for(byte j = 0; j <= wordCount-1; j++) {
			Gbl::strPtr->println(j);
			Gbl::strPtr->println(wordPtrs[j]);
		}
#endif
}

bool Controller::checkForRepeat(char *dataAr)  {
#ifdef DEBUG
	Gbl::strPtr->println(F("checkForRepeat"));
	Gbl::freeRam();
#endif
	if ('\0' == dataAr[0] || '\r' == dataAr[0]) {
		Gbl::strPtr->println(F("repeat"));
        return true;
    }
	Gbl::strPtr->println(F("no repeat"));
    return false;
}

bool Controller::checkForMode()  {
#ifdef DEBUG
    Gbl::strPtr->println(F("checkForMode"));
	Gbl::freeRam();
#endif
	if (strcasecmp(wordPtrs[0], "lights") == 0) {
        mode = LIGHTS;
    } else if (strcasecmp(wordPtrs[0], "status") == 0) {
        mode = STATUS;
    } else if (strcasecmp(wordPtrs[0], "com") == 0) {
        mode = COM;
    } else if (strcasecmp(wordPtrs[0], "clock") == 0
    		|| strcasecmp(wordPtrs[0], "timer") == 0) {
        mode = CLOCK;
    } else {
    	return false;
    }
	outputMode();
	Gbl::strPtr->println(F(" engaged"));
	return true;
}

void Controller::outputMode() {
	switch (mode) {
	case LIGHTS:
		Gbl::strPtr->print(F("LIGHTS Mode"));
		break;
	case STATUS:
		Gbl::strPtr->print(F("STATUS Mode"));
		break;
	case COM:
		Gbl::strPtr->print(F("COM Mode"));
		break;
	case CLOCK:
		Gbl::strPtr->print(F("CLOCK Mode"));
		break;
	}
}

bool Controller::processSerial(byte firstWord) {
#ifdef DEBUG
	Gbl::strPtr->print(F("processSerial "));
	Gbl::freeRam();
#endif
    switch (mode) {
#ifdef LTS
    case LIGHTS:
        return lightCtr.actionSerial(&wordPtrs[firstWord], wordCount-firstWord);
#endif
#ifdef STAT
    case STATUS:
        return statusCtr.actionSerial(&wordPtrs[firstWord], wordCount-firstWord);
#endif
#ifdef TME
    case CLOCK:
        return timeCtr.actionSerial(&wordPtrs[firstWord], wordCount-firstWord);
#endif
    }
        return false;
}


void Controller::irReceive(unsigned long inValue){
    static int hCount;
    if (inValue == IR_HOLD) {
        hCount++;
        irDecode(storedCode, hCount);
    } else {
        hCount = 0;
        irDecode(inValue, hCount);
    }
    delay(200);
}


bool Controller::isNum(char *word) {
	boolean isNum=true;
    const char *p;
	p = word;
	while (*p) {
		isNum = (
			isDigit(*p) ||
			*p == '+' ||
			*p == '.' ||
			*p == '-'
		);
		if(!isNum) return false;
		p++;
	}
    return true;
}

#ifdef IR
void Controller::irDecode(unsigned long inValue, int inHCount){
    bool actioned = false;
    switch (iRMode){
        case IR_LIGHTS:
            lightCtr.holdCount = inHCount;
            actioned = lightCtr.actionRemote(inValue);
            break;
        case IR_MP3:
            //MP3Remote.holdCount = inHCount;
            actioned = lightCtr.actionRemote(inValue);
            break;
    }
    if (true == actioned) {
        storedCode = inValue;
    } else {
    	Gbl::strPtr->print(inValue, HEX);
        storedCode = 0;
    }
}
#endif

