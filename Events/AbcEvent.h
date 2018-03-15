/*
 * Event.h
 *
 *  Created on: 15 Mar 2018
 *      Author: Adam
 */

#ifndef EVENTS_ABCEVENT_H_
#define EVENTS_ABCEVENT_H_
#include "../TimeCtr.h"
#include  "DS3231.h"

class AbcEvent {
public:
	AbcEvent(Time, bool, int);
	//virtual ~AbcEvent();

private:
	Time time;
	bool repeat;
	int delay;
	//virtual void action(int) = 0;
	//virtual void virtualMethod() = 0;
};

class HeaterOn: public AbcEvent {
public:
	void action(int);
};

class HeaterOff: public AbcEvent {
public:
	void action();
};

class LightsOn: public AbcEvent {
public:
	void action(int);
};

class LightsOff: public AbcEvent {
public:
	void action(int);
};

#endif /* EVENTS_ABCEVENT_H_ */


