/*
 * StatusCtr.h
 *
 *  Created on: 14 Nov 2017
 *      Author: Adam Milward
 */

#include "VoltMeter.h"
#include "TimeCtr.h"
#include "Storage.h"

#ifndef STATUSCTR_H_
#define STATUSCTR_H_

class StatusCtr {
    public:
        StatusCtr();
        StatusCtr(TimeCtr*);

        enum ReportType {REPORT, CSV};
        ReportType reportType = REPORT;

        uint8_t reportDelaySec = 0;
        uint32_t reportWaitMillis = 0;
        uint32_t storeWaitMillis = 0;
        uint8_t storeDelaySec = 4;

        bool actionSerial(char **, byte);
        void report();
        void csv();
        void timer(unsigned long);

    private:

        TimeCtr* timeCtrPtr;
        VoltMeter voltMeter;
        Storage store;
        void setReportDelay(byte);
        void doStorage();
        bool set(char **wordPtrs, byte wordCount);
        void setAll(float);
        void showCalibration();
};
#endif /* STATUSCTR_H_ */
