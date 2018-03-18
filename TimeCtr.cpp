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
	pinMode(Gbl::HEATER, OUTPUT);
	digitalWrite(Gbl::HEATER, LOW);

	water.ID = WATER;
	water.pin = Gbl::WATER;
	pinMode(Gbl::WATER, OUTPUT);
	digitalWrite(Gbl::WATER, LOW);
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
		if (!actionSet(&wordPtrs[1], --wordCount)) return help();
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
    	if (!setUtility(&wordPtrs[0], wordCount)) return help();
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
			(unsigned long)(millis - prevReportMillis) >= reportDelaySec * 1000)
    {
		prevReportMillis = millis;
		report();
	} else if (alarmsDelaySec
			&&
			(unsigned long)(millis - prevAlarmsMillis) >= alarmsDelaySec * 1000)
    {
		prevAlarmsMillis = millis;
		alarmsTimer();
	}
}

void TimeCtr::alarmsTimer() {
	uint32_t unixTime = clock->getUnixTime(clock->getTime());
	checkAlarm(heater, unixTime);
	checkAlarm(water, unixTime);
	checkAlarm(lights, unixTime);
}

void TimeCtr::checkAlarm(Utility &util, uint32_t unixTime) {
	if (ON == util.status) {
		// on mode, check timer mode to turn off
		if (unixTime >= util.timer.timeStamp) {
			utilOff(util);
		}
	} else if (OFF == util.status && util.alarm.active) {
		// off mode, check alarm to turn on
		if (unixTime >= util.alarm.timeStamp) {
			utilAlarmAction(util, unixTime);
		}
	}
}

void TimeCtr::utilAlarmAction(Utility &util, uint32_t unixTime) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::heaterOn"));
	Gbl::strPtr->print(F("unixTime "));
	Gbl::strPtr->println(unixTime);
	Gbl::freeRam();
#endif
	utilActivateTimer(util, util.alarm.timerMins, unixTime);
	utilResetAlarm(util);
}

void TimeCtr::utilResetAlarm(Utility &util) {
	if (util.alarm.repeat) {
		util.alarm.active = true;
		util.alarm.timeStamp = util.alarm.timeStamp + DAY_SECONDS;
	} else {
		utilAlarmOff(util);
	}
}

void TimeCtr::utilActivateTimer(Utility &util, byte duration, uint32_t unixTime) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::activateTimer"));
	Gbl::strPtr->print(F("duration "));
	Gbl::strPtr->println(duration);
	debugOutput(util);
	Gbl::freeRam();
#endif
	digitalWrite(util.pin, HIGH);
	util.status = ON;
	util.timer.timeStamp = (duration) ?
			(unixTime + HEATER_DELAY) : (unixTime + duration); // * 60 todo
	debugOutput(util);
}

void TimeCtr::utilOff(Utility &util) {
	debugOutput(util);
	digitalWrite(util.pin, LOW);
	util.status = OFF;
	debugOutput(util);
}

bool TimeCtr::actionSet(char **wordPtrs, byte wordCount) {
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
    prevReportMillis = millis();
}

bool TimeCtr::setUtility(char** wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::setUtility"));
	Gbl::freeRam();
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
		utilityReport(*utilPtr);
		return true;
	} else if (strcasecmp(wordPtrs[1], "on") == 0) {
		return utilitySetOn(*utilPtr, &wordPtrs[2], wordCount - 2);
	} else if (strcasecmp(wordPtrs[1], "off") == 0) {
		return utilitySetOff(*utilPtr, &wordPtrs[2], wordCount - 2);
	} else if (strcasecmp(wordPtrs[1], "alarm") == 0) {
		return utilitySetAlarm(*utilPtr, &wordPtrs[2], wordCount - 2);
	}

	return false;
}

void TimeCtr::utilityReport(Utility &util) {
	uint32_t unixTime = clock->getUnixTime(clock->getTime());
	int32_t seconds = util.alarm.timeStamp - unixTime;

	Gbl::strPtr->println();
	Gbl::strPtr->println(unixTime);
	Gbl::strPtr->println(util.alarm.timeStamp);
	Gbl::strPtr->println(seconds);
	Gbl::strPtr->println();

	int hours = seconds/60/60;
	int mins = (seconds/60) % 60;

	Gbl::strPtr->println(
			(util.ID == HEATER) ? F("HEATER") : (
				(util.ID == WATER) ? F("WATER") : (
					(util.ID == LIGHTS) ? F("LIGHTS") : F("dunno")
	)));
	if (ON == util.status) {
		Gbl::strPtr->print(F("ON for "));
		Gbl::strPtr->print(
			(util.timer.timeStamp - unixTime) / 60
		);
		Gbl::strPtr->println(F(" min"));
	} else {
		Gbl::strPtr->println(F("OFF"));
	}
	Gbl::strPtr->print(F("alarm "));
	Gbl::strPtr->println((util.alarm.active) ? F(" ON") : F(" OFF"));
	Gbl::strPtr->print(F("set for "));
	Gbl::strPtr->println(getTimeStr(util.alarm.h, util.alarm.m));
	Gbl::strPtr->println((util.alarm.repeat) ? F("repeat") : F("once"));
	Gbl::strPtr->print(F("On in "));
	Gbl::strPtr->println(getTimeStr(hours, mins));
	Gbl::strPtr->println(seconds);

	Gbl::strPtr->println();



	Gbl::strPtr->println(util.ID);
	Gbl::strPtr->println(util.pin);
	Gbl::strPtr->println((util.status == OFF) ? "off" : "on");
	Gbl::strPtr->println((util.alarm.active) ? "active" : "inactive");
	Gbl::strPtr->println((util.alarm.repeat) ? "repeat" : "once");
	Gbl::strPtr->println(util.alarm.timerMins);
	Gbl::strPtr->println(util.alarm.timeStamp);
	Gbl::strPtr->println(util.alarm.h);
	Gbl::strPtr->println(util.alarm.m);
	Gbl::strPtr->println(util.timer.timeStamp);
}

bool TimeCtr::utilitySetOn(Utility& util, char** wordPtrs, byte wordCount) {
	if (!wordCount) {
		utilActivateTimer(util, 60, clock->getUnixTime(clock->getTime()));
	} else if (1 == wordCount && Controller::isNum(wordPtrs[0])) {
		utilActivateTimer(util, atoi(wordPtrs[0]), clock->getUnixTime(clock->getTime()));
	} else {
		return false;
	}
	return true;
}

bool TimeCtr::utilitySetOff(Utility& util, char** wordPtrs, byte wordCount) {
	if (!wordCount) {
		utilOff(util);
	} else if (1 == wordCount && Controller::isNum(wordPtrs[0])) {
		utilActivateTimer(util, atoi(wordPtrs[0]), clock->getUnixTime(clock->getTime()));
	} else {
		return false;
	}
	return true;
}

void TimeCtr::debugOutput(Utility& util) {
		Gbl::strPtr->println(util.ID);
		Gbl::strPtr->println(util.pin);
		Gbl::strPtr->println((util.status == OFF) ? "off" : "on");
		Gbl::strPtr->println((util.alarm.active) ? "active" : "inactive");
		Gbl::strPtr->println((util.alarm.repeat) ? "repeat" : "once");
		Gbl::strPtr->println(util.alarm.timerMins);
		Gbl::strPtr->println(util.alarm.timeStamp);
		Gbl::strPtr->println(util.alarm.h);
		Gbl::strPtr->println(util.alarm.m);
		Gbl::strPtr->println(util.timer.timeStamp);
}

bool TimeCtr::utilitySetAlarm(Utility& util, char** wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::setAlarm"));
	Gbl::freeRam();
	Gbl::strPtr->println(wordCount);
	Gbl::strPtr->println(wordPtrs[0]);
	Gbl::strPtr->println(wordPtrs[1]);
	Gbl::strPtr->println(wordPtrs[2]);
#endif
	if (0 == wordCount) {
		if (strcasecmp(wordPtrs[0], "off") == 0) {
			Gbl::strPtr->println(F("alarm off"));
			utilAlarmOff(util);
			return true;
		} else if (strcasecmp(wordPtrs[0], "repeat") == 0) {
			Gbl::strPtr->println(F("alarm repeat"));
			util.alarm.repeat = true;
			return true;
		} else if (strcasecmp(wordPtrs[0], "once") == 0) {
			Gbl::strPtr->println(F("alarm once"));
			util.alarm.repeat = false;
			return true;
		}
	} else if (wordCount >= 2) {
		if (Controller::isNum(wordPtrs[0])
			&& Controller::isNum(wordPtrs[1]))
		{
			if (2 == wordCount) {
				return utilityConfigAlarm(
					util,
					atoi(wordPtrs[0]),
					atoi(wordPtrs[1])
				);
			} else if (3 == wordCount && Controller::isNum(wordPtrs[2])) {
				return utilityConfigAlarm(
					util,
					atoi(wordPtrs[0]),
					atoi(wordPtrs[1]),
					atoi(wordPtrs[2])
				);
			}
		}
	}
	return false;
}

bool TimeCtr::utilityConfigAlarm(
		Utility& util,
		byte h,
		byte m,
		byte timerMins
) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::ConfigAlarm"));
	Gbl::freeRam();
	Gbl::strPtr->println(h);
	Gbl::strPtr->println(m);
	Gbl::strPtr->println(timerMins);
#endif
	if (h > 23 || m > 59) {
		return false;
	}
	util.alarm.active = true;
	util.alarm.h = h;
	util.alarm.m = m;
	util.alarm.timerMins = timerMins;

	Time time = clock->getTime();
	uint32_t currentUnixTime = clock->getUnixTime(time);
	time.hour = h;
	time.min = m;
	uint32_t alarmUnixTime = clock->getUnixTime(time);

	if (alarmUnixTime < currentUnixTime) {
		alarmUnixTime += DAY_SECONDS;
	}
	util.alarm.timeStamp = alarmUnixTime;

	utilityReport(util);
	return true;
}

void TimeCtr::utilAlarmOff(Utility& util) {
	util.alarm.active = false;
	util.alarm.timeStamp = 0;
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
	Gbl::strPtr->println(clock->getUnixTime(clock->getTime()));

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
