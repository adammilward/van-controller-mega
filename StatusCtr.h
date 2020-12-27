/*
 * StatusCtr.h
 *
 *  Created on: 14 Nov 2017
 *      Author: Adam Milward
 */

#include "VoltMeter.h"

#ifndef STATUSCTR_H_
#define STATUSCTR_H_

class StatusCtr {
public:
    StatusCtr();

    enum ReportType {REPORT, CSV};
    ReportType reportType = REPORT;

    byte reportDelaySec = 0;
    unsigned long waitMillisReport = 0;


    bool actionSerial(char **, byte);
    void report();
    void csv();
    void timer(unsigned long);
private:

    TimeCtr* timeCtrPtr;
    VoltMeter voltMeter;
    void setReportDelay(byte);
    bool set(char **wordPtrs, byte wordCount);
    void setAll(float);
    void showCalibration();
};
#endif /* STATUSCTR_H_ */
