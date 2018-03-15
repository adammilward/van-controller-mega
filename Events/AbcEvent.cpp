/*
 * Event.cpp
 *
 *  Created on: 15 Mar 2018
 *      Author: Adam
 */

#include "../TimeCtr.h"
#include "AbcEvent.h"

AbcEvent::AbcEvent(Time inTime, bool inRepeat, int inDelay = 0) {
	time = inTime;
	repeat = inRepeat;
	delay = inDelay;
}

void HeaterOn::action(int delay = 60 * 60) {
	TimeCtr::heaterOn(delay);
}

void HeaterOff::action() {
	TimeCtr::heaterOff();
}

void LightsOn::action(int delay = 60 * 60) {
	TimeCtr::lightsFadeOn(delay);
}

void LightsOff::action(int delay = 60) {
	TimeCtr::lightsFadeOff(delay);
}
