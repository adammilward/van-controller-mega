#include "Arduino.h"

#ifndef STORAGE_H_
#define STORAGE_H_

class Storage {
    private:

        static const uint8_t numRecords = 60;

        struct analogRecord {
            uint32_t timestamp;
            float temp[numRecords];
            float vA0[numRecords];
        } oneMin, fiveMins, thirtyMins;

        uint32_t storeCount = 0;
        
        void addRecord(
            analogRecord &recordRef,
            uint32_t timestamp,
            float temp,
            float vA0
        );
        void outputRecords(analogRecord &recordRef);

    public:
        Storage();
        void output();
        void makeRecord(uint32_t, float, float);
};

#endif