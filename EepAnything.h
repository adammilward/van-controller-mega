/*
 * eepromAnything.h
 *
 *  Created on: 2 Apr 2018
 *      Author: Litlle Trollop
 */

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions
#include "Gbl.h"

#ifndef EEPADMIN_H_
#define EEPADMIN_H_

class EepAnything {
	public:
		static int const Gbl_AnalogPins_Start = 0;
		static int const TimeCtr_Utilities_Start = 20;
		static int const Next = 47;

};

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
	Gbl::strPtr->print(F("WRITING ee: "));
	Gbl::strPtr->println(ee);
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
	Gbl::strPtr->print(F("NEXT ee: "));
	Gbl::strPtr->println(ee);
    return ee;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

#endif /* EEPADMIN_H_ */
