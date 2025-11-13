#include "Arduino.h"

#ifndef STORAGE_H_
#define STORAGE_H_

class Storage {
    public:
        Storage();
        void output();
        void makeRecord(uint32_t, float*, uint8_t);
    
        static const uint8_t numSignals = 3;
    
    private:
        static const uint8_t numRecords = 60;

        struct analogRecord {
            uint32_t timestamp;
            float records[numSignals][numRecords];
            uint8_t nAverage;
        } oneMin, fiveMins, thirtyMins;

        uint32_t storeCount = 0;
        
        void addRecord(
            analogRecord &recordRef,
            uint32_t timestamp,
            float *,
            uint8_t 
        );

        float* averageRecords(
            analogRecord &,
            uint32_t,
            uint8_t,
            float*
        );

        void outputRecords(analogRecord &recordRef);
};

#endif