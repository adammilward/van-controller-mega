#include "Storage.h"
#include "Gbl.h"

Storage::Storage() {
    uint8_t i = 60;
    while (i --) {
        oneMin.vA0[i] = fiveMins.vA0[i] = thirtyMins.vA0[i] =
        oneMin.temp[i] = fiveMins.temp[i] = thirtyMins.temp[i] = 0;
    }
}

void Storage::makeRecord(
    uint32_t timestamp,
    float temp,
    float vA0
) {

    // thirty minute denominator
    if (! (storeCount % 30)) {
        addRecord(thirtyMins, timestamp, temp, vA0); 
    }

    // 5 minute denominator
    if (! (storeCount % 5)) {
        addRecord(fiveMins, timestamp, temp, vA0);
    }

    // every minute
    addRecord(oneMin, timestamp, temp, vA0);

    // increment counter
    storeCount ++;
    //reset counter once largest denominator reached
    if (storeCount == 30) {
        storeCount = 0;
    }
}

void Storage::addRecord(
    analogRecord &recordRef,
    uint32_t timestamp,
    float temp,
    float a0
) {
    recordRef.timestamp = timestamp;

    uint8_t i = numRecords;
    // copy elements from 58 - 0 into 59 - 1
    while (--i) {
        recordRef.temp[i] = recordRef.temp[i - 1];
        recordRef.vA0[i] = recordRef.vA0[i - 1];
    }
    // record the voltage
    recordRef.temp[0] = temp;
    recordRef.vA0[0] = a0;
}

void Storage::output() {
    Gbl::strPtr->println(F("<{'type': 'status', 'payload': {'records': {"));
    Gbl::strPtr->print(F("'oneMin': "));
    outputRecords(oneMin);
    Gbl::strPtr->print(F(",'fiveMins': "));
    outputRecords(fiveMins);
    Gbl::strPtr->print(F(",'thirtyMins': "));
    outputRecords(thirtyMins);
    Gbl::strPtr->print(F("}}}>"));
}

void Storage::outputRecords(analogRecord &recordRef) {
    Gbl::strPtr->print(F("{'timestamp': "));
    Gbl::strPtr->println(recordRef.timestamp);
    
    Gbl::strPtr->print(F(",'temp': ["));
    Gbl::strPtr->print(recordRef.temp[0]);
    for(uint8_t i = 1; i < (numRecords); i++) {
        Gbl::strPtr->print(F(","));    
        Gbl::strPtr->print(recordRef.temp[i]);
    }
    Gbl::strPtr->println(F("]"));

    
    Gbl::strPtr->print(F(",'vA0': ["));
    Gbl::strPtr->print(recordRef.vA0[0]);
    for(uint8_t i = 1; i < (numRecords); i++) {
        Gbl::strPtr->print(F(","));    
        Gbl::strPtr->print(recordRef.vA0[i]);
    }
    Gbl::strPtr->println(F("]}"));
}

