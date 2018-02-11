/*
 * timeCtr.h
 *
 *  Created on: 5 Feb 2018
 *      Author: Adam
 */

#include "Arduino.h"
#include "Light.h"
#include "DS3231.h"

#ifndef TIMECTR_H_
#define TIMECTR_H_

class TimeCtr {
public:
	TimeCtr();

	static Light *red;
	static Light *green;
	static Light *blue;

	bool actionSerial(char **, byte);
	void timer(unsigned long);

	static DS3231 *time;
private:

	byte reportDelay = 0;
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

};

#endif /* TIMECTR_H_ */
