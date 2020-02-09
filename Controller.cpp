
/*
 * RemoteReceive.cpp
 *
 *  Created on: 20 Feb 2017
 *      Author: Adam Milward
 */
#define DEBUG
#include "Controller.h"


//#define STAT
#define LTS
//#define TME




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
		StatusCtr statusCtr;
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
	if (serialGetCommand()) {
		Gbl::strPtr->println("serialRead read full command");
		Gbl::strPtr->println("letter at start of last word");
		Gbl::strPtr->println(wordPtrs[wordCount - 1]);
		Gbl::strPtr->println(wordPtrs[wordCount - 1][0]);
		Gbl::strPtr->println((byte)wordPtrs[wordCount - 1][0]);
		
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
				Gbl::strPtr->println(F("Modes are: 'lights', 'status', 'com' or 'clock'"));
				outputMode();
			}
		}

		Gbl::strPtr->println(F("---------string was terminated---------"));
		Gbl::strPtr->print(F("wordCoutnt: "));
		Gbl::strPtr->println(wordCount);
		Gbl::strPtr->print(F("commandLength: "));
		Gbl::strPtr->println(dataArrLength);
		for(byte j = 0; j <= wordCount-1; j++) {
			Gbl::strPtr->println(j);
			Gbl::strPtr->println(wordPtrs[j]);
		}
		wordCount = 1;
		wordPtrs[0] = dataArr;
		dataArrLength = 0;
	}

	Gbl::strPtr->println(F("---------state of string after read---------"));
	Gbl::strPtr->print(F("wordCoutnt: "));
	Gbl::strPtr->println(wordCount);
	Gbl::strPtr->print(F("commandLength: "));
	Gbl::strPtr->println(dataArrLength);
	for(byte j = 0; j <= wordCount-1; j++) {
			Gbl::strPtr->println(j);
			Gbl::strPtr->println(wordPtrs[j]);
		}
	Gbl::strPtr->println("********************end serialRead******");	
}

bool Controller::serialGetCommand() {
#ifdef DEBUG
	Gbl::strPtr->println(F("serialRead"));
	//Gbl::freeRam();
#endif

	char newChar = Gbl::strPtr->read();

	switch (newChar) {
		case '\r':
		case '\n':
			Gbl::strPtr->println(F("found terminator"));
			terminateCommand();
			return true;
		case '\0':
		case ' ':
		 	Gbl::strPtr->println(F("found space"));	
			addSpace();
			return false;
		default:
		 	Gbl::strPtr->print(F("found: "));	
		 	Gbl::strPtr->print(newChar);
			addChar(newChar);
			return false;	
	}
}

void Controller::addChar(char newChar) {
	dataArr[dataArrLength ++] = newChar;
}

void Controller::addSpace() {
	Gbl::strPtr->println(dataArr[dataArrLength]);
	Gbl::strPtr->println((byte) dataArr[dataArrLength - 1]);
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

// 	Gbl::strPtr->print("totalLenght: ");
// 	Gbl::strPtr->println(totalLength);
// 	int i = 0;
// 	while (i < newLength) {
// 			Gbl::strPtr->print("recieved: ");
// 			Gbl::strPtr->print(dataAr[i]);
// 			Gbl::strPtr->print(" - ");
// 			Gbl::strPtr->println((byte) dataAr[i ++]);
// 	}



// 	if ('\r' != dataAr[totalLength -1]
// 		|| '\n' != dataAr[totalLength -1])
// 	{
// 		Gbl::strPtr->println("command finished");
// 	} else {
// 		Gbl::strPtr->println("command not finished");
// 	}

// 	if (! checkForRepeat(dataAr)) {
		
// 		// set the space Flag to true, this it the first input
// 		// to record the start of the first word
// 		bool spaceFlag = (! prevDataLength);
// 		for(byte i = prevDataLength; i < (totalLength); i++ ) {
// 			// loop, set all spaces to null terminator recording into data member
// 			prevDataAr[i] = dataAr[i - prevDataLength];
// 			// record the position of each word
// 			Gbl::strPtr->print(F("dataAr at position i: "));
// 			Gbl::strPtr->println(prevDataAr[i]);
// 			if (prevDataAr[i] == ' ' || prevDataAr[i] == '\0' || wordCount == maxWords+1) {
// 				prevDataAr[i] = '\0';
// 				spaceFlag = true;
// 				Gbl::strPtr->println("found Space");
// 			} else if ('\r' == prevDataAr[i]
// 				|| '\n' == prevDataAr[i]
// 			) {
// 				prevDataAr[i] = '\0';
// 				spaceFlag = true;
// 				prevDataLength = 0;
// 				commandTerminated = true;
// 				Gbl::strPtr->println("command terminated");
// 				Gbl::strPtr->println("found return");
// 			} else if (spaceFlag == true) {
// 				wordPtrs[wordCount] = prevDataAr + i;
// 				spaceFlag = false;
// 				wordCount++;
// 				Gbl::strPtr->print("space flag new word: ");
// 				Gbl::strPtr->println(wordCount);
// 			}
// 		}
// 		// can have no more than 5 words
// 		if (wordCount > maxWords) wordCount = maxWords;
// 	} else {
// 		return true;
// 	}
// #ifdef DEBUG
// 		Gbl::strPtr->print(F("wordCoutnt: "));
// 		Gbl::strPtr->println(wordCount);
// 		Gbl::strPtr->print(F("commandLength: "));
// 		Gbl::strPtr->println(totalLength);
// 		for(byte j = 0; j <= wordCount-1; j++) {
// 			Gbl::strPtr->println(j);
// 			Gbl::strPtr->println(wordPtrs[j]);
// 		}
// #endif
// 	prevDataLength = totalLength;
// 	return commandTerminated;


bool Controller::checkForRepeat(char *dataAr)  {
#ifdef DEBUG
	Gbl::strPtr->println(F("checkForRepeat"));
	Gbl::freeRam();
#endif
	if ('\0' == dataAr[0] || '\r' == dataAr[0] || '\n' == dataAr[0]) {
		Gbl::strPtr->println(F("repeat true"));
        return true;
    }
		Gbl::strPtr->println(F("reoeat false"));
    return false;
}

bool Controller::checkForMode()  {
#ifdef DEBUG
    Gbl::strPtr->println(F("checkForMode"));
	Gbl::freeRam();
#endif
	if (
		strcasecmp(wordPtrs[0], "l") == 0
		|| strcasecmp(wordPtrs[0], "lights") == 0)
	{
        mode = LIGHTS;
    } else if (
		strcasecmp(wordPtrs[0], "s") == 0
		|| strcasecmp(wordPtrs[0], "status") == 0)
	{
        mode = STATUS;
    } else if (
		strcasecmp(wordPtrs[0], "c") == 0
		|| strcasecmp(wordPtrs[0], "t") == 0
		|| strcasecmp(wordPtrs[0], "clock") == 0
		|| strcasecmp(wordPtrs[0], "time") == 0
    	|| strcasecmp(wordPtrs[0], "timer") == 0)
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
	//case COM:
		//Gbl::strPtr->print(F("COM Mode"));
		//break;
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

