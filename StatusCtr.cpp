/*
 * StatusCtr.cpp
 *
 *  Created on: 14 Nov 2017
 *      Author: Adam Milward
 */
#include "Controller.h"
#include "StatusCtr.h"
#include "VoltMeter.h"
#include "Arduino.h" // todo remove

//#define DEBUG

StatusCtr::StatusCtr(){
	VoltMeter voltMeter;
}

bool StatusCtr::actionSerial(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
	Gbl::strPtr->println(F("StatusCtr::actionSerial"));
	Gbl::freeRam();
#endif
	if (0 == wordCount) {
		return true;
	}
	if (strcasecmp(wordPtrs[0], "report") == 0) {
        if (wordCount == 2 && Controller::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
			reportType = REPORT;
		} else {
			report();
		}
    } else if (strcasecmp(wordPtrs[0], "csv") == 0) {
        if (wordCount == 2 && Controller::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
			reportType = CSV;
        } else {
        	csv();
        }
    } else if (strcasecmp(wordPtrs[0], "raw") == 0) {
    	Gbl::strPtr->println(F("raw"));
    	voltMeter.toggleConfigMode();
    } else if (strcasecmp(wordPtrs[0], "set") == 0) {
		set(&wordPtrs[1], --wordCount);
    } else if (strcasecmp(wordPtrs[0], "save") == 0) {
    	Gbl::strPtr->println(F("save"));
    	voltMeter.saveCalibration();
    } else if (strcasecmp(wordPtrs[0], "calibration") == 0) {
    	Gbl::strPtr->println(F("calibration"));
    	showCalibration();
    } else {
    	Gbl::strPtr->println(F("Status Controller commands are:"));
    	Gbl::strPtr->println(F("report [nn]"));
    	Gbl::strPtr->println(F("csv [nn]"));
    	Gbl::strPtr->println(F("raw"));
    	Gbl::strPtr->println(F("save"));
    	Gbl::strPtr->println(F("calibration"));
    	Gbl::strPtr->println(F("set"));
        return false;
    }
    return true;

}

void StatusCtr::showCalibration() {
#ifdef DEBUG
		Gbl::strPtr->print(F("StatusCtr::showCalibration"));
#endif
    char buff[14];
    for (int pin = 0; pin < Gbl::noAnalogPins; pin++) {
        dtostrf(voltMeter.getCalibration(pin), 4, 12, buff);
        Gbl::strPtr->println(buff);
    }
}

bool StatusCtr::set(char **wordPtrs, byte wordCount) {
#ifdef DEBUG
		Gbl::strPtr->print(F("StatusCtr::set"));
#endif
	for (byte i = 0; i < wordCount; i++) {
#ifdef DEBUG
		Gbl::strPtr->print(F("word number:"));
		Gbl::strPtr->println(i);
		Gbl::strPtr->println(wordPtrs[i]);
#endif
		if (Controller::isNum(wordPtrs[i])) {
			//
		} else {
			Gbl::strPtr->println("no");
		}
	}

	if (wordCount == 2) {
        if (strcasecmp(wordPtrs[0], "all") == 0 && Controller::isNum(wordPtrs[1])) {
            setAll(atof(wordPtrs[1]));
            return true;
        } else if (Controller::isNum(wordPtrs[0])  &&  Controller::isNum(wordPtrs[1])) {
            voltMeter.setPin(atoi(wordPtrs[0]), atof(wordPtrs[1]));
            Gbl::strPtr->println(F("value set"));
            return true;
        }
    }
    Gbl::strPtr->println(F("Set Failed, command format is: "));
    Gbl::strPtr->println(F("Set All (nn.nn)"));
    Gbl::strPtr->println(F("Set (pinNumber) (nn.nn)"));
    return false;
}

void StatusCtr::setAll(float newValue) {
#ifdef DEBUG
		Gbl::strPtr->print(F("StatusCtr::setAll"));
#endif
    for (int pin = 0; pin < 5; pin++) {
        voltMeter.setPin(pin, newValue);
    }
    Gbl::strPtr->print(F("all set to "));
    Gbl::strPtr->println(newValue);
}

void StatusCtr::setReportDelay(byte delaySeconds) {
#ifdef DEBUG
		Gbl::strPtr->print(F("StatusCtr::setReportDelay"));
#endif
    reportDelaySec = delaySeconds;
}

void StatusCtr::timer(unsigned long millis) {
    if (reportDelaySec
			&&
			(unsigned long)(millis - waitMillisReport)
				>= (unsigned long)reportDelaySec * 1000)
        {
            if (REPORT == reportType) {
                report();
            } else {
                csv();
            }
            waitMillisReport = millis;
        }
}

void StatusCtr::report() {
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->print(F("Solar Panels: "));
    Gbl::strPtr->print(voltMeter.getVoltage(0));
    Gbl::strPtr->println(F("V"));
    Gbl::strPtr->print(F("Consumer Unit: "));
    Gbl::strPtr->print(voltMeter.getVoltage(1));
    Gbl::strPtr->println(F("V"));
    Gbl::strPtr->print(F("Leisue Battery: "));
    Gbl::strPtr->print(voltMeter.getVoltage(2));
    Gbl::strPtr->println(F("V"));
    Gbl::strPtr->print(F("Van Battery: "));
    Gbl::strPtr->print(voltMeter.getVoltage(3));
    Gbl::strPtr->println(F("V"));
    Gbl::strPtr->print("spare: ");
    Gbl::strPtr->print(voltMeter.getVoltage(4));
    Gbl::strPtr->println(F("V"));
    Gbl::strPtr->print(F("delay= "));
    Gbl::strPtr->println(reportDelaySec);
	Gbl::freeRam();
}

void StatusCtr::csv() {
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->print(voltMeter.getVoltage(0));
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->print(voltMeter.getVoltage(1));
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->print(voltMeter.getVoltage(2));
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->print(voltMeter.getVoltage(3));
    Gbl::strPtr->print(F(", "));
    Gbl::strPtr->println(voltMeter.getVoltage(4));
}






















