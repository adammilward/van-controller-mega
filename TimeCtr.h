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

	bool help();

	void heaterOn(int);
	void heaterOff(int);
	void waterOn(int);
	void waterOff(int);
	void lightsFadeOn(int);
	void lightsFadeOff(int);

	typedef void (TimeCtr::*actionFcn)(int);

	enum Status {ON, OFF};
	enum UtilID {LIGHTS, HEATER, WATER};

	struct Alarm {
		bool active = false;
		bool repeat = false;
		actionFcn onPtr;
		uint8_t fcnParam = 60;
		uint32_t timeStamp = 0;
		uint8_t h = 0;
		uint8_t m = 0;
	};
	struct Timer {
		actionFcn offPtr;
		uint8_t fcnParam = 60;
		uint32_t timeStamp = 0;
		uint8_t durationMins = 0;
	};
	struct Utility {
		UtilID ID;
		byte pin;
		Status status = OFF;
		Alarm alarm;
		Timer timer;
	};

	actionFcn testPtr;

	Utility heater;
	Utility water;
	Utility lights;

	byte reportDelaySec = 0;
	unsigned long waitMillisReport = 0;

	void report();
	void setReportDelay(byte);

	bool set(char**, byte);
	bool setDay(char**, byte);
	bool setDate(char**, byte);
	bool setTime(char**, byte);

	void outTime();
	void outDate();
	void outTemp();

	bool timer(char**, byte);
	void utilityReport(Utility*);
	bool utilitySetOn(Utility*, char**, byte);
	bool utilitySetOff(Utility*, char**, byte);
	bool utilitySetAlarm(Utility*, char**, byte);

	void utilitySetTimer(Utility*, int);
	void utilityOff(int);

	void timerOn(Timer);
	void timerOff(Timer, byte);
	void timerOn(Timer, byte, byte);
	void timerOff(Timer, byte, byte);
	void timerAlarm(Timer, byte, byte);

	bool setAlarm();

	char *getTimeStr(byte, byte);
};

#endif /* TIMECTR_H_ */
