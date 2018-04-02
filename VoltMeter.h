/*
 * VoltMeter.h
 *
 *  Created on: 18 Nov 2017
 *      Author: Adam Milward
 */

#include "Arduino.h"
#include "Gbl.h"
#include "EEPROM.h"

#ifndef VOLTMETER_H_
#define VOLTMETER_H_

class VoltMeter {
public:
    VoltMeter();

    enum ConfigMode {OFF = 0, ON = 1};
    ConfigMode configMode = OFF;

    float convFactor[Gbl::noAnalogPins] = {
        Gbl::A0_FACTOR,
        Gbl::A1_FACTOR,
        Gbl::A2_FACTOR,
        Gbl::A3_FACTOR,
        Gbl::A4_FACTOR
    };

    void toggleConfigMode();
    float getVoltage(int pinNumber);
    uint16_t readPin(byte);
    void setPin(int, float);
    void saveCalibration();
    void retrieveCalibration();
    float getCalibration(int);
};


#endif /* VOLTMETER_H_ */
