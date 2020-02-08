/*
 * timeCtr.h
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "Arduino.h"
#include "Light.h"
#include "DS3231.h"
#include "LightCtr.h"
#include "EepAnything.h"

#ifndef TIMECTR_H_
#define TIMECTR_H_

class TimeCtr{
public:
	TimeCtr();

	bool actionSerial(char **, byte);
	void actionTimer(unsigned long);

	static DS3231* clock;

private:
	static const byte HEATER_DELAY = 60;
	static const uint32_t DAY_SECONDS = 86400;
	byte reportDelaySec = 0;
	unsigned long prevReportMillis = 0;
	static const byte INITIAL_ALARMS_DELAY = 1;
	byte alarmsDelaySec = INITIAL_ALARMS_DELAY;
	unsigned long prevAlarmsMillis = 0;

	enum Status {ON, OFF};
	enum UtilID {LED, HEATER, WATER};

	static const byte ALARM_TIMER_MINS_MAX = 120;
	static const byte ALARM_TIMER_MINS_MIN = 30;
	static const byte LED_ALARM_TIMER_MINS_MIN = 5;

	struct Alarm {
		bool active = false;
		bool repeat = false;
		byte timerMins = 30;
		uint32_t timeStamp = -1; //set too high to activate
		byte h = 0;
		byte m = 0;
	};
	struct Timer {
		uint32_t timeStamp = 0; //set low (turn off)
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
	Utility led;

	bool help();

	void readEepAlarm(Utility&);
	void writeEepAlarm(Utility&);

	void alarmsTimer();
	void checkAlarm(Utility&, uint32_t);
	void utilAlarmAction(Utility&, uint32_t);
	void utilResetAlarm(Utility&);
	void utilActivateTimerOn(Utility&, byte, uint32_t);
	void utilActivateTimerOff(Utility&, byte, uint32_t);
	void utilOff(Utility&);

	void utilityReport(Utility&);

	bool setUtility(char**, byte);
	bool utilitySetOn(Utility&, char**, byte);
	bool utilitySetOff(Utility&, char**, byte);
	bool utilitySetAlarm(Utility&, char**, byte);
	bool utilityConfigAlarm(Utility&, byte, byte, byte timerMins = 0);
	void utilityValidate(Utility&);

	// decide how on and off perform
	void utilOffAction(Utility &);
	void utilOnAction(Utility &, byte);

	void report();
	void setReportDelay(byte);

	bool actionSet(char**, byte);
	bool setDay(char**, byte);
	bool setDate(char**, byte);
	bool setTime(char**, byte);
	void outTime();
	void outDate();
	void outTemp();

	char *getTimeStr(byte, byte, bool = false);
};

#endif /* TIMECTR_H_ */
