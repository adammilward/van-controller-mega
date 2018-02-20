/*
 * Alarm.h
 *
 *  Created on: 18 Feb 2018
 *      Author: Adam
 */
#include "DS3231.h"

#ifndef UTILITIES_ALARM_H_
#define UTILITIES_ALARM_H_

namespace Utilities {

class Alarm : public Time {
public:
	Alarm();
	bool on;
	bool repeat;

	byte actions[4];


};

} /* namespace VanTime */

#endif /* UTILITIES_ALARM_H_ */
