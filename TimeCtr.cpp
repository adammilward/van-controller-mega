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
	utilOffAction(heater);

	water.ID = WATER;
	water.pin = Gbl::WATER;
	pinMode(Gbl::WATER, OUTPUT);
	utilOffAction(water);

	led.ID = LED;
	led.pin = 225;
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
    		|| strcasecmp(wordPtrs[0], "led") == 0)
    {
    	if (! setUtility(&wordPtrs[0], wordCount)) return help();
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
	Gbl::strPtr->println(F("heater|water|led"));
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
	checkAlarm(led, unixTime);
}

void TimeCtr::checkAlarm(Utility &util, uint32_t unixTime) {
	if (ON == util.status) {
		// on mode, check timer mode to turn off
		if (unixTime >= util.timer.timeStamp) {
			utilOff(util);
		}
	}
	if (util.alarm.active) {
		// off mode, check alarm to turn on
		if (unixTime >= util.alarm.timeStamp) {
			utilAlarmAction(util, unixTime);
		}
	}
	if (unixTime >= util.alarm.timeStamp) {
		Gbl::strPtr->println(F("DANGER"));
		Gbl::strPtr->println(F("unixTime > util.alarm.timeStamp"));
		utilityReport(util);
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
	util.alarm.timeStamp = util.alarm.timeStamp + DAY_SECONDS;
	util.alarm.active = util.alarm.repeat;
}

void TimeCtr::utilActivateTimer(Utility &util, byte duration, uint32_t unixTime) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::utilActivateTimer"));
	Gbl::strPtr->print(F("duration "));
	Gbl::strPtr->println(duration);
	Gbl::freeRam();
#endif
	utilOnAction(util);
	util.status = ON;
	util.timer.timeStamp = (duration) ?
			(unixTime + duration) : (unixTime + HEATER_DELAY); // * 60 todo
	utilityReport(util);
}

void TimeCtr::utilOff(Utility &util) {
	utilityReport(util);
	utilOffAction(util);
	util.status = OFF;
	utilityReport(util);
}

void TimeCtr::utilOffAction(Utility &util) {
	if (LED == util.ID) {

	} else {
		digitalWrite(util.pin, LOW);;
	}
}

void TimeCtr::utilOnAction(Utility &util) {
	if (LED == util.ID) {

	} else {
		digitalWrite(util.pin, HIGH);
	}
}

bool TimeCtr::actionSet(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("TimeCtr::set"));
	Gbl::freeRam();
	for (byte i = 0; i < wordCount; i++) {
		Gbl::strPtr->print(F("word number:"));
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
		Gbl::strPtr->print(F("word number:"));
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
		Gbl::strPtr->println(F("LEDs"));
		utilPtr = &led;
	}

	bool retVal = false;
// todo check worod count hear
	// eg 1, 2 , 3

	if (1 == wordCount) {
		// this is called later by default
		//utilityReport(*utilPtr);
		retVal = true;
	} else if (strcasecmp(wordPtrs[1], "on") == 0) {
		retVal = utilitySetOn(*utilPtr, &wordPtrs[2], wordCount - 2);
	} else if (strcasecmp(wordPtrs[1], "off") == 0) {
		retVal = utilitySetOff(*utilPtr, &wordPtrs[2], wordCount - 2);
	} else if (strcasecmp(wordPtrs[1], "alarm") == 0) {
		retVal = utilitySetAlarm(*utilPtr, &wordPtrs[2], wordCount - 2);
	}

	if (retVal) utilityReport(*utilPtr);
	return retVal;
}

void TimeCtr::utilityReport(Utility &util) {
	uint32_t unixTime = clock->getUnixTime(clock->getTime());
	int32_t seconds = util.alarm.timeStamp - unixTime;
// todo sort out secontd > 1 day
	int hours = seconds/60/60;
	int mins = (seconds/60) % 60;

	Gbl::strPtr->println(
			(util.ID == HEATER) ? F("HEATER") : (
				(util.ID == WATER) ? F("WATER") : (
					(util.ID == LED) ? F("LEDs") : F("dunno")
	)));
	if (ON == util.status) {
		Gbl::strPtr->print(F("ON for: "));
		Gbl::strPtr->print(
			(util.timer.timeStamp - unixTime) / 60
		);
		Gbl::strPtr->println(F(" min"));
	} else {
		Gbl::strPtr->println(F("OFF"));
	}
	Gbl::strPtr->println();
	Gbl::strPtr->print(F("Alarm: "));
	if (util.alarm.active) {
		Gbl::strPtr->print(F("will come on on in: "));
		Gbl::strPtr->print(getTimeStr(hours, mins, true));
		Gbl::strPtr->println(F("m "));
	} else {
		Gbl::strPtr->println(F("off"));
	}
	Gbl::strPtr->print(F("set for "));
	Gbl::strPtr->println(getTimeStr(util.alarm.h, util.alarm.m));
	Gbl::strPtr->println((util.alarm.repeat) ? F("repeat") : F("once"));
	Gbl::strPtr->print(F("on duration min: "));
	Gbl::strPtr->println(util.alarm.timerMins);

	Gbl::strPtr->println();
	Gbl::strPtr->print(F("unixTime                   "));
	Gbl::strPtr->println(unixTime);
	Gbl::strPtr->print(F("util.alarm.timeStamp       "));
	Gbl::strPtr->println(util.alarm.timeStamp);
	Gbl::strPtr->print(F("util.timer.timeStamp       "));
	Gbl::strPtr->println(util.timer.timeStamp);
	Gbl::strPtr->print(F("alarm.timeStamp - unixTime "));
	Gbl::strPtr->println(seconds);
	Gbl::strPtr->println();

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

void TimeCtr::debugOutput(Utility& util) { // todo remove
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
	for (byte i = 0; i < wordCount; i++) {
		Gbl::strPtr->print(F("word count: "));
		Gbl::strPtr->println(wordCount);
		Gbl::strPtr->print(F("word number: "));
		Gbl::strPtr->println(i);
		Gbl::strPtr->println(wordPtrs[i]);
	}
#endif
	if (1 == wordCount) {
		if (strcasecmp(wordPtrs[0], "off") == 0) {
			Gbl::strPtr->println(F("alarm off "));
			util.status = OFF;
			return true;
		} else if (strcasecmp(wordPtrs[0], "on") == 0) {
			Gbl::strPtr->println(F("alarm on "));
			util.alarm.active = true;
			if (util.alarm.timeStamp <= clock->getUnixTime(clock->getTime())) {
				util.alarm.timeStamp += DAY_SECONDS;
			}
			return true;
		} else if (strcasecmp(wordPtrs[0], "repeat") == 0) {
			Gbl::strPtr->println(F("alarm repeat "));
			util.alarm.repeat = true;
			return true;
		} else if (strcasecmp(wordPtrs[0], "once") == 0) {
			Gbl::strPtr->println(F("alarm once "));
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
		int timerMins
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
	util.alarm.timerMins = (timerMins > 255) ? 255 : timerMins;

	Time time = clock->getTime();
	uint32_t currentUnixTime = clock->getUnixTime(time);
	time.hour = h;
	time.min = m;
	uint32_t alarmUnixTime = clock->getUnixTime(time);

	if (alarmUnixTime <= currentUnixTime) {
		alarmUnixTime += DAY_SECONDS;
	}
	util.alarm.timeStamp = alarmUnixTime;

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

char *TimeCtr::getTimeStr(byte h, byte m, bool longFormat)
{
    static char output[] = "xxxxxxx";
    if (h<10)
        output[0]=48;
    else
        output[0]=char((h / 10)+48);
    output[1]=char((h % 10)+48);

    byte i = 2;
    if (longFormat) {
    	output[i++] = 'h';
    	output[i++] = ' ';
    } else {
    	output[i++] = 58;
    }
    if (m<10)
        output[i++]=48;
    else
        output[i++]=char((m / 10)+48);
    output[i++]=char((m % 10)+48);
    output[i++]=0;
    return output;
}
