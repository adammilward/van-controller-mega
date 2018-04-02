
/*
 * VoltMeter.cpp
 *
 *  Created on: 18 Nov 2017
 *      Author: Adam Milward
 */
#define DEBUG
#include "VoltMeter.h"
#include "EepAnything.h"

VoltMeter::VoltMeter() {
    retrieveCalibration();
}

void VoltMeter::toggleConfigMode() {
#ifdef DEBUG
	Gbl::strPtr->println(F("VoltMeter::toggleConfigMode"));
	Gbl::freeRam();
#endif
    configMode = (configMode == OFF) ? ON : OFF;
}

float VoltMeter::getVoltage(int pinNumber) {
    return (configMode) ?
            readPin(pinNumber) :
            readPin(pinNumber)*convFactor[pinNumber];
}

uint16_t VoltMeter::readPin(byte pinNo) {
	uint16_t analogValue = 0;
	byte i = 2;
	while (i--) {
		delay(20);
		analogValue += analogRead(pinNo);
	}
	i = 5;
	while (i--) {
		delay(20);
		analogValue += analogRead(pinNo);
	}
	return analogValue /= 5;
}

void VoltMeter::setPin(int pin, float newValue) {
#ifdef DEBUG
	Gbl::strPtr->println(F("VoltMeter::setPin"));
	Gbl::freeRam();
#endif
    convFactor[pin] = newValue/readPin(pin);
    Gbl::strPtr->print(pin);
    Gbl::strPtr->print(F(" set to "));
    Gbl::strPtr->println(newValue);
}

float VoltMeter::getCalibration(int pin) {
#ifdef DEBUG
	Gbl::strPtr->println(F("VoltMeter::getCalibration"));
	Gbl::freeRam();
#endif
    return convFactor[pin];
}

void VoltMeter::saveCalibration() {
#ifdef DEBUG
	Gbl::strPtr->println(F("VoltMeter::saveCalibration"));
	Gbl::freeRam();
#endif
	int ee = 0;
	int address = 0;
    for (int pin = 0; pin < Gbl::noAnalogPins; pin++) {
    	ee = pin*sizeof(convFactor[pin]);
		address = EEPROM_writeAnything(ee, convFactor[pin]);
    }
	Gbl::strPtr->print(F("Next Address Start: "));
	Gbl::strPtr->println(address);
}

void VoltMeter::retrieveCalibration() {
	for (int pin = 0; pin < Gbl::noAnalogPins; pin++) {
        int ee = pin*sizeof(convFactor[pin]);
        EEPROM_readAnything(ee, convFactor[pin]);
    }
}


























