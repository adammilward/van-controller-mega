#include "Arduino.h"

#ifndef STORAGE_H_
#define STORAGE_H_

class Storage {
    private:

        static const uint8_t numRecords = 60;

        struct analogRecord {
            uint16_t records[numRecords];
            uint32_t timestamp;
        } oneMin, fiveMins, thirtyMins;

        uint32_t sampleTimestamp = 1609691391;
        uint32_t storeCount = 0;
        
        void addRecord(
            uint16_t vRaw,
            uint32_t timestamp,
            analogRecord &recordRef
        );
        void outputRecords(analogRecord &recordRef);

    public:
        Storage();
        void output();
        void makeRecord();
};

#endif