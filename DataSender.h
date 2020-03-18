#ifndef DATASENDER_H
#define DATASENDER_H

#include "DevKitMQTTClient.h"
#include "AzureIotHub.h"
#include "parson.h"
#include "IoT_DevKit_HW.h"

#include "data.h"

class DataSender
{
public:
    DataSender(const SensorData* source, uint32_t updateTime);

    void Update(uint32_t deltaTime);

    void Enable(bool isEnabled);

private:
    bool initialized;
    bool enabled;

    uint32_t updateCounter;
    uint32_t timeBetweenUpdates;

    const SensorData* sourceData;

    uint32_t messagesSent;
};

#endif