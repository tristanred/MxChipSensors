#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "AzureIotHub.h"
#include "parson.h"
#include "IoT_DevKit_HW.h"

#include "data.h"
#include "StatusBar.h"
#include "DataSender.h"

#define LOOP_DELAY 10

static bool hasWifi = false;
static bool hasIoTHub = false;

static bool is_paused = false;

static SensorData* sensors = NULL;
static StatusBar* statusUpdater = NULL;
static DataSender* dataSender = NULL;

uint64_t previous_loop_time;
uint64_t current_loop_time;

void setup()
{
    initIoTDevKit(true);

    setup_comms();

    if(hasWifi == false)
    {
      Screen.clean();
      Screen.print("Err: No WIFI.");

      return;
    }

    if(hasIoTHub == false)
    {
      Screen.clean();
      Screen.print("Err: No IotHub.");

      return;
    }

    previous_loop_time = SystemTickCounterRead();
    current_loop_time = previous_loop_time;

    sensors = new SensorData();
    memset(sensors, 0, sizeof(SensorData));

    statusUpdater = new StatusBar(sensors, 100);

    dataSender = new DataSender(sensors, 15000);

    Screen.clean();
    Screen.print("Init complete !");
}

void loop()
{
    // Keep track of loop times
    previous_loop_time = current_loop_time;
    current_loop_time = SystemTickCounterRead();

    uint64_t time_diff = current_loop_time - previous_loop_time;

    // Update the screen. Because we use earlier returns it would be complicated
    // to update the screen and/or check if the screen need an update in-between
    // the sensor loops. Just use whatever data was previously loaded.
    statusUpdater->Update((uint32_t)time_diff);

    // Check if Button A is pressed to pause/unpause sensor collection.
    int btn_a_state = getButtonAState();

    if(is_paused == true && btn_a_state == 1)
    {
        is_paused = false;

        Screen.clean();
        Screen.print("Unpaused");

        delay(5000);
    }
    else if (is_paused == true)
    {
        delay(LOOP_DELAY);

        return;
    }
    else if(is_paused == false && btn_a_state == 1)
    {
        is_paused = true;

        Screen.clean();
        Screen.print("Paused");

        delay(5000);

        return;
    }

    // If we're setup to send data to the cloud, check the sensors and
    // upload the data. Avoid collecting data if there's nowhere to send it to.
    if (hasIoTHub && hasWifi)
    {
        fill_sensordata(sensors);

        Serial.print("Filled sensor data.\n");

        dataSender->Update(time_diff);
    }

    delay(LOOP_DELAY);
}

void setup_comms()
{
    if (WiFi.begin() == WL_CONNECTED)
    {
        hasWifi = true;
        Screen.print(1, "Running...");

        DevKitMQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "SensorDev");

        if (!DevKitMQTTClient_Init())
        {
            hasIoTHub = false;
            return;
        }

        DevKitMQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

        hasIoTHub = true;
    }
    else
    {
        hasWifi = false;
        Screen.print(1, "No Wi-Fi");
    }
}

void fill_sensordata(struct SensorData* data)
{
    data->humidity = getDevKitHumidityValue();
    data->temperature = getDevKitTemperatureValue(0);
    data->pressure = getDevKitPressureValue();
    getDevKitMagnetometerValue(&data->mag_x, &data->mag_y, &data->mag_z);
    getDevKitGyroscopeValue(&data->gyro_x, &data->gyro_y, &data->gyro_z);
    getDevKitAcceleratorValue(&data->accel_x, &data->accel_y, &data->accel_z);
}

static int DeviceMethodCallback(const char* methodName, const unsigned char* payload, int size, unsigned char** response, int* response_size)
{
    if(strcmp(methodName, "start"))
    {
        Screen.print(0, "Unpaused.");

        is_paused = false;
    }
    else if(strcmp(methodName, "pause"))
    {
        Screen.print(0, "Paused.");

        is_paused = true;
    }

    return 0;
}