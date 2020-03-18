#include "StatusBar.h"

StatusBar::StatusBar(SensorData* source, int loopTime)
{
    initialized = false;

    if(source == NULL || loopTime == 0)
    {
        return;
    }

    this->sourceData = source;
    this->timePerPage = loopTime;
    this->currentPage = 0;
    this->pageAccumulatedTime = 0;

    initialized = true;
}

void StatusBar::Update(uint32_t deltaTime, bool forceRefresh)
{
    if(initialized == false)
    {
        return;
    }

    // Accumulate time and if enough time has passed, pass to the next page
    // and update the screen.

    pageAccumulatedTime += deltaTime;
    if(pageAccumulatedTime >= timePerPage)
    {
        currentPage++;

        if(currentPage >= PAGES_REQUIRED - 1)
        {
            currentPage = 0;
        }
    }
    else if (forceRefresh == false)
    {
        // If not enough time has ellapsed, keep the same content on the
        // screen.
        return;
    }

    char lineOne[16];
    char lineTwo[16];

    switch(currentPage)
    {
        case HUMIDITY:
        {
            snprintf(lineOne, 16,"Humidity");
            snprintf(lineTwo, 16, "%.2f", this->sourceData->humidity);

            break;
        }
        case TEMPERATURE:
        {
            snprintf(lineOne, 16,"Temperature");
            snprintf(lineTwo, 16, "%.2f c", this->sourceData->temperature);

            break;
        }
        case PRESSURE:
        {
            snprintf(lineOne, 16,"Pressure");
            snprintf(lineTwo, 16, "%i kpi", (int)this->sourceData->temperature);

            break;
        }
        case MAGNETIC:
        {
            snprintf(lineOne, 16,"Magnetic");
            snprintf(lineTwo, 16, "x=%i,y=%i,z=%i", (int)this->sourceData->mag_x, (int)this->sourceData->mag_y, (int)this->sourceData->mag_z);

            break;
        }
        case GYRO:
        {
            snprintf(lineOne, 16,"Gyroscope");
            snprintf(lineTwo, 16, "x=%i,y=%i,z=%i", (int)this->sourceData->gyro_x, (int)this->sourceData->gyro_y, (int)this->sourceData->gyro_z);

            break;
        }
        case ACCELEROMETER:
        {
            snprintf(lineOne, 16,"Accelerometer");
            snprintf(lineTwo, 16, "x=%i,y=%i,z=%i", (int)this->sourceData->accel_x, (int)this->sourceData->accel_y, (int)this->sourceData->accel_z);

            break;
        }
        default:
        {
            currentPage = 0;

            snprintf(lineOne, 16, "Page select");
            snprintf(lineTwo, 16, "error.");

            break;
        }
    }

    Screen.print(1, lineOne, false);
    Screen.print(2, lineTwo, false);
}