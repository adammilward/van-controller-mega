/*
 * timeCtr.h
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "Arduino.h"
#include "Utilities/Light.h"
#include "DS3231.h"
#include "Events/AbcEvent.h"


#ifndef TIMECTR_H_
#define TIMECTR_H_

class TimeCtr{
public:
	TimeCtr();

	static Light *red;
	static Light *green;
	static Light *blue;

	bool actionSerial(char **, byte);
	bool help();
	void timer(unsigned long);

	static DS3231 *clock;

	static void heaterOn(int);
	static void heaterOff();
	static void waterHeaterOn(int);
	static void waterHeaterOff();
	static void lightsFadeOn(int);
	static void lightsFadeOff(int);

private:

	byte reportDelay = 0;
	unsigned long waitMillisReport = 0;

	AbcEvent heaterEvent;

	void report();
	void setReportDelay(byte);

	bool set(char**, byte);
	bool setDay(char**, byte);
	bool setDate(char**, byte);
	bool setTime(char**, byte);

	void outTime();
	void outDate();
	void outTemp();

	bool setAlarm();
};

#endif /* TIMECTR_H_ */
