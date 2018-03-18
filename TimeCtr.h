/*
 * timeCtr.h
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "Arduino.h"
#include "Utilities/Light.h"
#include "DS3231.h"



#ifndef TIMECTR_H_
#define TIMECTR_H_

class TimeCtr{
public:
	TimeCtr();

	static Light *red;
	static Light *green;
	static Light *blue;

	bool actionSerial(char **, byte);
	void actionTimer(unsigned long);

	static DS3231 *clock;

private:



	static const byte HEATER_DELAY = 60;
	static const uint32_t DAY_SECONDS = 86400;
	byte reportDelaySec = 0;
	unsigned long prevReportMillis = 0;
	byte const alarmsDelaySec = 15;
	unsigned long prevAlarmsMillis = 0;

	enum Status {ON, OFF};
	enum UtilID {LIGHTS, HEATER, WATER};

	struct Alarm {
		bool active = false;
		bool repeat = false;
		byte timerMins = 60;
		uint32_t timeStamp = 0;
		byte h = 0;
		byte m = 0;
	};
	struct Timer {
		uint32_t timeStamp = 0;
	};
	struct Utility {
		UtilID ID;
		byte pin;
		Status status = OFF;
		Alarm alarm;
		Timer timer;
	};

	Utility heater;
	Utility water;
	Utility lights;

	bool help();

	void alarmsTimer();
	void checkAlarm(Utility&, uint32_t);
	void utilAlarmAction(Utility&, uint32_t);
	void utilActivateTimer(Utility&, byte, uint32_t);
	void utilResetAlarm(Utility&);
	void utilOff(Utility&);

	void utilAlarmOff(Utility&);

	bool setUtility(char**, byte);
	void utilityReport(Utility&);
	bool utilitySetOn(Utility&, char**, byte);
	bool utilitySetOff(Utility&, char**, byte);
	bool utilitySetAlarm(Utility&, char**, byte);
	bool utilityConfigAlarm(Utility&, byte, byte, byte timerMins = HEATER_DELAY);
	void utilitySetTimer(Utility&, int);


	void debugOutput(Utility&);
	void report();
	void setReportDelay(byte);

	bool actionSet(char**, byte);
	bool setDay(char**, byte);
	bool setDate(char**, byte);
	bool setTime(char**, byte);
	void outTime();
	void outDate();
	void outTemp();

	char *getTimeStr(byte, byte);
};

#endif /* TIMECTR_H_ */
