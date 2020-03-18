#ifndef DATA_H
#define DATA_H

struct SensorData
{
    float humidity;
    float temperature;
    float pressure;

    int mag_x;
    int mag_y;
    int mag_z;

    int gyro_x;
    int gyro_y;
    int gyro_z;

    int accel_x;
    int accel_y;
    int accel_z;
};

struct BoardData
{
  const char* devKitName;
  const char* serialNumber;
  const char* connString;
};

#endif