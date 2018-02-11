/*
 * timeCtr.cpp
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "Arduino.h"
#include "TimeCtr.h"
#include "Gbl.h"
#include "Controller.h"
#include "DS3231.h"

#define DEBUG

TimeCtr::TimeCtr() {



}



bool TimeCtr::actionSerial(char **wordPtrs, byte wordCount) {
	Gbl::strPtr->println(F("TimeCtr::actionSerial"));
	Gbl::freeRam();

	if (strcasecmp(wordPtrs[0], "report") == 0) {
        Gbl::strPtr->println(F("report"));
        if (wordCount == 2 && Controller::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
		}
		report();
    } else if (strcasecmp(wordPtrs[0], "set") == 0) {
		set(&wordPtrs[1], --wordCount);
    } else {
    	Gbl::strPtr->println(F("Time Controller commands are:"));
    	Gbl::strPtr->println(F("report, time, date, alarm"));
    	Gbl::strPtr->println(F("set time hh mm ss"));
    	Gbl::strPtr->println(F("set date dd mm yyyy"));
    	Gbl::strPtr->println(F("set day (1-7)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) time hh mm ss"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) date (daily|dd mm yyyy)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) (on|off)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) (on|off)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) heater (off|nn)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) shower (off|nn)"));
    	Gbl::strPtr->println(F("set alarm(1|2|3) lights (off|nn)"));
        return false;
    }
    return true;
}

void TimeCtr::timer(unsigned long millis) {
    if (reportDelay > 0
			&&
		(signed long)(millis - waitMillisReport) >= 0)
    {
		report();
		waitMillisReport = millis + reportDelay;
	}
}

bool TimeCtr::set(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::set"));
	Gbl::freeRam();
	for (byte i = 0; i < wordCount; i++) {
		Gbl::strPtr->print("word number:");
		Gbl::strPtr->println(i);
		Gbl::strPtr->println(wordPtrs[i]);
	}
#endif

	if (strcasecmp(wordPtrs[0], "day") == 0) {
		return setDay(&wordPtrs[1], wordCount-1);
	} else if (strcasecmp(wordPtrs[0], "date")) {
		//return setDate(&wordPtrs[1], wordCount-1);
	} else if (strcasecmp(wordPtrs[0], "time")) {
		//return setTime(&wordPtrs[1], wordCount-1);
	} else {

	}

    Gbl::strPtr->println(F("Set Failed, command format is: "));
    Gbl::strPtr->println(F("Set All (nn.nn)"));
    Gbl::strPtr->println(F("Set (pinNumber) (nn.nn)"));
    return false;
}

bool TimeCtr::setDay(char **wordPtrs, byte wordCount) {
	if (Controller::isNum(wordPtrs[1])) {
		time->setDOW(atoi(wordPtrs[1]));
		Gbl::strPtr->print(F("day set to: "));
		Gbl::strPtr->println(time->getDOWStr());
		return true;
	}
	return false;
}

void TimeCtr::setReportDelay(byte delayMillis) {
    reportDelay = (unsigned long)(delayMillis * 1000);
    waitMillisReport = millis() + reportDelay;
}

void TimeCtr::report() {
    Gbl::strPtr->println(F("Time Report"));
    outTime();
    outDate();
    outTemp();

    // Send Unixtime for 00:00:00 on January 1th 2014
    /*Gbl::strPtr->print("Alarm set for ");
    Gbl::strPtr->print(time->getDOWStr(alarm));
    Gbl::strPtr->print(" ");

    Gbl::strPtr->print(" ");
    Gbl::strPtr->print(time->getDateStr(alarm));
    Gbl::strPtr->print(" -- ");
    Gbl::strPtr->println(time->getTimeStr(alarm));*/
}

void TimeCtr::outTime() {
    // Send time
    Gbl::strPtr->print(F("Time: "));
    Gbl::strPtr->println(time->getTimeStr());
}

void TimeCtr::outDate() {
    // Send time
    Gbl::strPtr->print(time->getDOWStr());
    Gbl::strPtr->print(F("Date: "));
    Gbl::strPtr->println(time->getDateStr());
    Gbl::strPtr->println(time->getMonthStr());
}
void TimeCtr::outTemp() {
    // Send time
    Gbl::strPtr->print(F("Temperature: "));
   	Gbl::strPtr->print(time->getTemp());
   	Gbl::strPtr->println(F(" C"));
}


