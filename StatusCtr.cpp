/*
 * StatusCtr.cpp
 *
 *  Created on: 14 Nov 2017
 *      Author: Adam Milward
 */

#include "StatusCtr.h"

//#define DEBUG


StatusCtr::StatusCtr() {}

StatusCtr::StatusCtr(TimeCtr* inTimeCtrPtr){
    VoltMeter voltMeter;
    timeCtrPtr = inTimeCtrPtr;
    store = Storage();
}

void StatusCtr::timer(unsigned long millis) {
    uint32_t elapsed = millis - storeWaitMillis;
    uint32_t delay = (uint32_t)storeDelaySec * 1000;
    float values[store.numSignals];

    values[0] = voltMeter.getVoltage(0);
    values[1] = voltMeter.getVoltage(1);
    values[2] = timeCtrPtr->getTemp();


    if (storeDelaySec && elapsed >= delay) {
        store.makeRecord(
            timeCtrPtr->getTimestamp(),
            values,
            store.numSignals
        );
        if (reportDelaySec) {
            store.output();
        }
        storeWaitMillis = millis;
    }
    
    elapsed = millis - reportWaitMillis;
    delay = (uint32_t)reportDelaySec * 1000;
    if (reportDelaySec && elapsed >= delay) {
        if (REPORT == reportType) {
            report();
        } else {
            csv();
        }
        reportWaitMillis = millis;
    }
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
        if (wordCount == 2 && Gbl::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
			reportType = REPORT;
		} else {
			report();
		}
    } else if (strcasecmp(wordPtrs[0], "csv") == 0) {
        if (wordCount == 2 && Gbl::isNum(wordPtrs[1])) {
			setReportDelay(atoi(wordPtrs[1]));
			reportType = CSV;
        } else {
        	csv();
        }
    } else if (strcasecmp(wordPtrs[0], "records") == 0) {
        store.output();
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
	    Gbl::strPtr->println(F("Command not recognised, options are:"));
    	Gbl::strPtr->println(F("Status Controller commands are:"));
    	Gbl::strPtr->println(F("report [nn]"));
    	Gbl::strPtr->println(F("csv [nn]"));
    	Gbl::strPtr->println(F("records"));
    	Gbl::strPtr->println(F("raw"));
    	Gbl::strPtr->println(F("save"));
    	Gbl::strPtr->println(F("calibration"));
    	Gbl::strPtr->println(F("Set {pinNumber} {nn.nn}"));
    	Gbl::strPtr->println(F("remember to 'save' after setting"));
        return false;
    }
    return true;

}

void StatusCtr::showCalibration() {
    for (int pin = 0; pin < Gbl::noAnalogPins; pin++) {
        Gbl::sprintf(voltMeter.getCalibration(pin), true, 15);
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
		if (Gbl::isNum(wordPtrs[i])) {
			//
		} else {
			Gbl::strPtr->println("no");
		}
	}

	if (wordCount == 2) {
        if (strcasecmp(wordPtrs[0], "all") == 0 && Gbl::isNum(wordPtrs[1])) {
            setAll(atof(wordPtrs[1]));
            return true;
        } else if (Gbl::isNum(wordPtrs[0])  &&  Gbl::isNum(wordPtrs[1])) {
            voltMeter.setPin(atoi(wordPtrs[0]), atof(wordPtrs[1]));
            Gbl::strPtr->println(F("value set"));
            Gbl::strPtr->println(F("remember to 'save' when done"));
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


void StatusCtr::report() {
    Gbl::strPtr->println(F("<{'type': 'status', 'payload': {"));
    Gbl::strPtr->print(F(" 'timestamp': "));
    Gbl::strPtr->println(timeCtrPtr->getTimestamp());
    Gbl::strPtr->print(F(",'temp': "));
    Gbl::strPtr->println(timeCtrPtr->getTemp());
    Gbl::strPtr->print(F(",'a0': "));
    Gbl::sprintf(voltMeter.getVoltage(0));
    Gbl::strPtr->print(F(",'a1': "));
    Gbl::sprintf(voltMeter.getVoltage(1));
    Gbl::strPtr->print(F(",'a2': "));
    Gbl::sprintf(voltMeter.getVoltage(2));
    Gbl::strPtr->print(F(",'a3': "));
    Gbl::sprintf(voltMeter.getVoltage(3));
    Gbl::strPtr->print(",'a4': ");
    Gbl::sprintf(voltMeter.getVoltage(4));
    Gbl::strPtr->print(F(",'delay': "));
    Gbl::sprintf(reportDelaySec);
    Gbl::strPtr->println(F("}}>"));
	Gbl::freeRam();

    
}

void StatusCtr::csv() {
    Gbl::strPtr->print(F(", "));
    Gbl::sprintf(voltMeter.getVoltage(0));
    Gbl::strPtr->print(F(", "));
    Gbl::sprintf(voltMeter.getVoltage(1));
    Gbl::strPtr->print(F(", "));
    Gbl::sprintf(voltMeter.getVoltage(2));
    Gbl::strPtr->print(F(", "));
    Gbl::sprintf(voltMeter.getVoltage(3));
    Gbl::strPtr->print(F(", "));
    Gbl::sprintf(voltMeter.getVoltage(4));
}
