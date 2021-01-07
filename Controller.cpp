
/*
 * RemoteReceive.cpp
 *
 *  Created on: 20 Feb 2017
 *      Author: Adam Milward
 */
//#define DEBUG
#include "Controller.h"

#define LTS
#define STAT
#define TME

#ifdef TME
	#include "TimeCtr.h"
#endif

#ifdef STAT
	#include "StatusCtr.h"
#endif

Controller::Controller() {
    // setup for reading commands
    dataArr[0] = '\0';
	wordCount = 1;
	wordPtrs[0] = dataArr;
	#ifdef LTS
		LightCtr lightCtr;
	#endif
	#ifdef TME
		TimeCtr timeCtr;
	#endif
	#ifdef STAT
		//todo if we may want to pass a null pointer, if timeCtr is not initialised
		StatusCtr statusCtr(&timeCtr);
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
	Serial.println(F("serialRecieve"));
	Gbl::freeRam();
#endif
	if (serialGetCommand()) {	
		if ('\0' == wordPtrs[wordCount - 1][0]) {
			wordCount --;
		}

		//if mode is set then start at the second word
		if (checkForMode()) {
			processSerial(1);
		} else {
			if (!processSerial(0)) {
				// if no mode and process failed
				Gbl::strPtr->println(F("I'm sorry Dave, I'm afraid I can't do that."));
				Gbl::strPtr->println(F("Modes are: 'lights', 'status' or 'time'"));
				outputMode();
			}
		}

		#ifdef DEBUG
			Serial.println(F("---------string was terminated---------"));
			Serial.print(F("wordCoutnt: "));
			Serial.println(wordCount);
			Serial.print(F("commandLength: "));
			Serial.println(dataArrLength);
			for(byte j = 0; j <= wordCount-1; j++) {
				Serial.println(j);
				Serial.println(wordPtrs[j]);
			}
		#endif
		wordCount = 1;
		wordPtrs[0] = dataArr;
		dataArrLength = 0;
	} else {
	#ifdef DEBUG
		Serial.println(F("---------state of string after read---------"));
		Serial.print(F("wordCoutnt: "));
		Serial.println(wordCount);
		Serial.print(F("commandLength: "));
		Serial.println(dataArrLength);
		for(byte j = 0; j <= wordCount-1; j++) {
				Serial.println(j);
				Serial.println(wordPtrs[j]);
			}
		Serial.println("********************end serialRead******");
	#endif
	}
}

bool Controller::serialGetCommand() {
	char newChar = Gbl::strPtr->read();
	#ifdef DEBUG
		Serial.println(F("serialGetCommand got char "));
		Serial.print(newChar);
		Serial.print(F(" - "));
		Serial.println((byte)newChar);
	#endif
	switch (newChar) {
		case '\r':
		case '\n':
			terminateCommand();
			return true;
		case '\0':
		case ' ':
			addSpace();
			return false;
		default:
			addChar(newChar);
			return false;	
	}
}

void Controller::addChar(char newChar) {
	dataArr[dataArrLength ++] = newChar;
}

void Controller::addSpace() {
	if ('\0' != dataArr[dataArrLength - 1]) {
		dataArr[dataArrLength ++] = '\0';
		// set following position to null, so next word is null (unless char is recieved);
		dataArr[dataArrLength] = '\0';
		// set the next wordPtr to the char after (may no be used)
		wordPtrs[wordCount ++] = dataArr + dataArrLength;
	}
}

void Controller::terminateCommand() {
	dataArr[dataArrLength ++] = '\0';
}

bool Controller::checkForMode()  {
	if (strcasecmp(wordPtrs[0], "l") == 0
	    ||strcasecmp(wordPtrs[0], "lights") == 0)
	{
        mode = LIGHTS;
    } else if (
		strcasecmp(wordPtrs[0], "s") == 0
		|| strcasecmp(wordPtrs[0], "status") == 0)
	{
        mode = STATUS;
    } else if (
		strcasecmp(wordPtrs[0], "t") == 0
		|| strcasecmp(wordPtrs[0], "time") == 0
	)
	{
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
	case CLOCK:
		Gbl::strPtr->print(F("CLOCK Mode"));
		break;
	}
}

bool Controller::processSerial(byte firstWord) {
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
