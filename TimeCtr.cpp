/*
 * timeCtr.cpp
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "TimeCtr.h"
#include "Arduino.h"
#include "Gbl.h"
#include "Controller.h"
#include "DS3231.h"

#define DEBUG

TimeCtr::TimeCtr() {
	heater.ID = HEATER;
	heater.pin = Gbl::HEATER;
	heater.alarm.onPtr = &TimeCtr::heaterOn;
	heater.timer.offPtr = &TimeCtr::heaterOff;
	testPtr = &TimeCtr::heaterOn;
}

bool TimeCtr::actionSerial(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::actionSerial"));
	Gbl::freeRam();
#endif

	if (strcasecmp(wordPtrs[0], "report") == 0) {
        if (wordCount == 2 && Controller::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
		}
		report();
    } else if (strcasecmp(wordPtrs[0], "set") == 0) {
		if (!set(&wordPtrs[1], --wordCount)) return help();
    } else if (strcasecmp(wordPtrs[0], "time") == 0) {
		outTime();
    } else if (strcasecmp(wordPtrs[0], "temp") == 0) {
		outTemp();
    } else if (strcasecmp(wordPtrs[0], "date") == 0) {
		outDate();
    } else if (strcasecmp(wordPtrs[0], "heater") == 0
    		|| strcasecmp(wordPtrs[0], "water") == 0
    		|| strcasecmp(wordPtrs[0], "lights") == 0)
    {
    	if (!timer(&wordPtrs[0], wordCount)) return help();
    } else {
    	return help();
    }
	return true;
}

bool TimeCtr::help() {
	Gbl::strPtr->println(F("Time Controller commands are:"));
	Gbl::strPtr->println(F("report|time|date"));
	Gbl::strPtr->println(F("set time hh mm ss"));
	Gbl::strPtr->println(F("set date dd mm yyyy"));
	Gbl::strPtr->println(F("set day (1-7)"));
	Gbl::strPtr->println(F("heater|water|lights"));
	Gbl::strPtr->println(F("thing on|off"));
	Gbl::strPtr->println(F("thing on|off mm"));
	Gbl::strPtr->println(F("thing on mm mm"));
	Gbl::strPtr->println(F("thing alarm hh mm [mm]"));
	return false;
}

void TimeCtr::actionTimer(unsigned long millis) {
    if (reportDelaySec
			&&
		(signed long)(millis - waitMillisReport) >= 0)
    {
		waitMillisReport = millis + reportDelaySec * 1000;
		report();
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
		if (setDay(&wordPtrs[1], wordCount-1)) return true;
	} else if (strcasecmp(wordPtrs[0], "date") == 0) {
		if (setDate(&wordPtrs[1], wordCount-1)) return true;
	} else if (strcasecmp(wordPtrs[0], "time") == 0) {
		if (setTime(&wordPtrs[1], wordCount-1)) return true;
	}

    Gbl::strPtr->println(F("Set Failed, command format is: "));
    Gbl::strPtr->println(F("day n | date dd mm yyyy | time hh mm ss"));
    return false;
}

bool TimeCtr::setDay(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::setDay"));
	Gbl::freeRam();
	for (byte i = 0; i < wordCount; i++) {
		Gbl::strPtr->print("word number:");
		Gbl::strPtr->println(i);
		Gbl::strPtr->println(wordPtrs[i]);
	}
#endif

	if (wordCount == 1 && Controller::isNum(wordPtrs[0])) {
		clock->setDOW(atoi(wordPtrs[0]));
	} else if (0 == wordCount) {
		clock->setDOW(atoi(wordPtrs[0]));
	} else {
		return false;
	}
	Gbl::strPtr->print(F("day set to: "));
	Gbl::strPtr->println(clock->getDOWStr());
	return true;
}

bool TimeCtr::setDate(char **wordPtrs, byte wordCount) {
	Gbl::strPtr->println(F("setDate"));
	if (
		wordCount == 3
		&& Controller::isNum(wordPtrs[0])
		&& Controller::isNum(wordPtrs[1])
		&& Controller::isNum(wordPtrs[2])
	) {
		clock->setDate(atoi(wordPtrs[0]), atoi(wordPtrs[1]), atoi(wordPtrs[2]));
		Gbl::strPtr->print(F("date set to: "));
		Gbl::strPtr->println(clock->getDateStr());
		return true;
	}
	return false;
}

bool TimeCtr::setTime(char **wordPtrs, byte wordCount) {
	Gbl::strPtr->println(F("setTime"));
	if (
		wordCount == 3
		&& Controller::isNum(wordPtrs[0])
		&& Controller::isNum(wordPtrs[1])
		&& Controller::isNum(wordPtrs[2])
	) {
		clock->setTime(atoi(wordPtrs[0]), atoi(wordPtrs[1]), atoi(wordPtrs[2]));
		Gbl::strPtr->print(F("time set to: "));
		Gbl::strPtr->println(clock->getTimeStr());
		return true;
	}
	return false;
}

void TimeCtr::setReportDelay(byte delaySec) {
    reportDelaySec = delaySec;
    waitMillisReport = millis() + reportDelaySec * 1000;
}

bool TimeCtr::timer(char** wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::timer"));
	Gbl::freeRam();
	Gbl::strPtr->println(wordPtrs[0]);
#endif
	Utility* utilPtr;
	if (strcasecmp(wordPtrs[0], "heater") == 0) {
		Gbl::strPtr->println(F("heater"));
		utilPtr = &heater;
	} else if (strcasecmp(wordPtrs[0], "water") == 0) {
		Gbl::strPtr->println(F("water"));
		utilPtr = &water;
	} else {
		Gbl::strPtr->println(F("lights"));
		utilPtr = &lights;
	}

	if (1 == wordCount) {
		utilityReport(utilPtr);
	} else if (strcasecmp(wordPtrs[1], "on") == 0) {
		return utilitySetOn(utilPtr, &wordPtrs[2], wordCount - 2);
	}

	return true;
}

void TimeCtr::utilityReport(Utility* util) {
	if (ON == util->status) {
		Gbl::strPtr->print(F("ON for"));
		Gbl::strPtr->print(
			(util->timer.timeStamp - clock->getUnixTime(clock->getTime())) / 60
		);
		Gbl::strPtr->println(F(" min"));
	} else {
		Gbl::strPtr->print(F("OFF"));
	}
	Gbl::strPtr->print(F("alarm "));
	Gbl::strPtr->println((util->alarm.active) ? F(" ON") : F(" OFF"));
	Gbl::strPtr->print(F("set for "));
	Gbl::strPtr->println(getTimeStr(util->alarm.h, util->alarm.m));
}

bool TimeCtr::utilitySetOn(Utility* util, char** wordPtrs, byte wordCount) {
	if (!wordCount) {
		utilitySetTimer(util, 60);
	} else if (1 == wordCount && Controller::isNum(wordPtrs[0])) {
		utilitySetTimer(util, atoi(wordPtrs[0]));
	} else {
		return false;
	}
	return true;
}

bool TimeCtr::utilitySetOff(Utility* util, char** wordPtrs, byte wordCount) {
	if (!wordCount) {
		utilityOff(0);
	} else if (1 == wordCount && Controller::isNum(wordPtrs[0])) {
		utilitySetTimer(util, atoi(wordPtrs[0]));
	} else {
		return false;
	}
	return true;
}

void TimeCtr::utilitySetTimer(Utility* util, int timeMins) {
	if (timeMins) {
		Gbl::strPtr->println(util->alarm.active);
		Gbl::strPtr->println(util->alarm.repeat);
		Gbl::strPtr->println(util->alarm.fcnParam);
		Gbl::strPtr->println(util->alarm.timeStamp);
		Gbl::strPtr->println(util->alarm.h);
		Gbl::strPtr->println(util->alarm.m);
		Gbl::strPtr->println(util->timer.fcnParam);
		Gbl::strPtr->println(util->timer.timeStamp);
		Gbl::strPtr->println(util->timer.durationMins);
		Gbl::strPtr->println(util->ID);
		Gbl::strPtr->println(util->pin);
		Gbl::strPtr->println(util->status);
		//(this->*testPtr)(60);
		(this->*util->alarm.onPtr)(60);


	} else {
		//(util->timer.*offPtr)(0);
	}
}

/*void TimeCtr::utilityOff(int timeMins) {
	if (timeMins) {

	}
}*/

bool TimeCtr::utilitySetAlarm(Utility* util, char** wordPtrs, byte wordCount) {
	return true;
}


void TimeCtr::report() {
    Gbl::strPtr->println(F("Time Report"));
    outTime();
    outDate();
    outTemp();
    if(reportDelaySec) {
    	Gbl::strPtr->print(F("report timer: "));
    	Gbl::strPtr->println(reportDelaySec);
    }

	//Gbl::strPtr->println(reinterpret_cast<void*>(help));

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
    Gbl::strPtr->println(clock->getTimeStr());
}

void TimeCtr::outDate() {
    // Send time
    Gbl::strPtr->print(F("Date: "));
    Gbl::strPtr->println(clock->getDateStr());
    Gbl::strPtr->print(clock->getDOWStr());
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->println(clock->getMonthStr());
}
void TimeCtr::outTemp() {
    // Send time
    Gbl::strPtr->print(F("Temp: "));
   	Gbl::strPtr->print(clock->getTemp());
   	Gbl::strPtr->println(F(" C"));
}

void TimeCtr::heaterOn(int param) {
   	Gbl::strPtr->println(F("HEATERON"));
	digitalWrite(Gbl::HEATER, HIGH);
	heater.status = ON;
}

void TimeCtr::heaterOff(int param) {
	digitalWrite(Gbl::HEATER, LOW);
	heater.status = OFF;
}

void TimeCtr::waterOn(int param) {
	digitalWrite(Gbl::WATER, HIGH);
	water.status = ON;
}

void TimeCtr::waterOff(int param) {
	digitalWrite(Gbl::WATER, HIGH);
	water.status = OFF;
}

void TimeCtr::lightsFadeOn(int delay) {

}

void TimeCtr::lightsFadeOff(int delay) {

}

char *TimeCtr::getTimeStr(byte h, byte m)
{
    static char output[] = "xxxxxx";
    if (h<10)
        output[0]=48;
    else
        output[0]=char((h / 10)+48);
    output[1]=char((h % 10)+48);
    output[2]=58;
    if (m<10)
        output[3]=48;
    else
        output[3]=char((m / 10)+48);
    output[4]=char((m % 10)+48);
    output[5]=0;
    return output;
}
