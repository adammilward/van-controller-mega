#include "Storage.h"
#include "Gbl.h"

Storage::Storage() {
    uint8_t i = 60;
    while (i --) {
        oneMin.records[i] = 0;
        fiveMins.records[i] = 0;
        thirtyMins.records[i] = 0;
    }
}

void Storage::makeRecord() {
    uint16_t vRaw = analogRead(A0);
    uint32_t timestamp = sampleTimestamp;

    // thirty minute denominator
    if (! (storeCount % 30)) {
        addRecord(vRaw, sampleTimestamp, thirtyMins); 
    }

    // 5 minute denominator
    if (! (storeCount % 5)) {
        addRecord(vRaw, sampleTimestamp, fiveMins);
    }

    // every minute
    addRecord(vRaw, sampleTimestamp, oneMin);

    // increment counter
    storeCount ++;
    //reset counter once largest denominator reached
    if (storeCount == 30) {
        storeCount = 0;
    }
}

void Storage::addRecord(
    uint16_t vRaw,
    uint32_t timestamp,
    analogRecord &recordRef
) {
    recordRef.timestamp = timestamp;

    uint8_t i = numRecords;
    // copy elements from 58 - 0 into 59 - 1
    while (--i) {
        recordRef.records[i] = recordRef.records[i - 1];
    }
    // record the voltage
    recordRef.records[0] = vRaw;
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
    Gbl::strPtr->print(F(",'records': ["));
    Gbl::strPtr->print(recordRef.records[0]);
    for(uint8_t i = 1; i < (numRecords); i++) {
        Gbl::strPtr->print(F(","));    
        Gbl::strPtr->print(recordRef.records[i]);
    }
    Gbl::strPtr->print(F("]}"));
}

