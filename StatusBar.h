#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "data.h"

#include <Arduino.h>

#define PAGES_REQUIRED 6

enum InfoPages
{
    HUMIDITY,
    TEMPERATURE,
    PRESSURE,
    MAGNETIC,
    GYRO,
    ACCELEROMETER
};

class StatusBar
{
public:
    StatusBar(SensorData* source, int loopTime);

    void Update(uint32_t deltaTime, bool forceRefresh = false);

private:
    bool initialized;

    SensorData* sourceData;

    int currentPage;

    int timePerPage;
    int pageAccumulatedTime;

};


#endif