#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "AzureIotHub.h"
#include "parson.h"

#include "IoT_DevKit_HW.h"

#define LOOP_DELAY 15000

static bool hasWifi = false;
static bool hasIoTHub = false;

static int msg_count = 0;

static bool is_paused = false;

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

    Screen.clean();
    Screen.print("Init complete !");
}

void loop()
{
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

    if (hasIoTHub && hasWifi)
    {
      SensorData data;
      memset(&data, 0, sizeof(SensorData));

      fill_sensordata(&data);

      Serial.print("Filled sensor data.\n");

      char msg[1024];
      int res = generate_message(&data, msg, 1024);

      Serial.printf("Message generated in %i bytes.\n", res);

      if(res > 0)
      {
        msg_count++;

        EVENT_INSTANCE* message = DevKitMQTTClient_Event_Generate(msg, MESSAGE);
        DevKitMQTTClient_SendEventInstance(message);

        Serial.print("MQTT message sent.\n");

        char msg_count_buf[32];
        sprintf(msg_count_buf, "Msg sent : %i", msg_count);
        Screen.print(0, msg_count_buf, false);
      }
      else
      {
        Serial.print("MQTT message error.\n");

        Screen.print(0, "Message error.");

        char err[24];
        sprintf(err, "%i bytes written.", res);
        Screen.print(1, err);
      }
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
        hasIoTHub = true;
    }
    else
    {
        hasWifi = false;
        Screen.print(1, "No Wi-Fi");
    }
}

void fill_sensordata(SensorData* data)
{
  data->humidity = getDevKitHumidityValue();
  data->temperature = getDevKitTemperatureValue(0);
  data->pressure = getDevKitPressureValue();
  getDevKitMagnetometerValue(&data->mag_x, &data->mag_y, &data->mag_z);
  getDevKitGyroscopeValue(&data->gyro_x, &data->gyro_y, &data->gyro_z);
  getDevKitAcceleratorValue(&data->accel_x, &data->accel_y, &data->accel_z);
}

int generate_message(SensorData* data, char* out_buf, size_t buf_len)
{
  JSON_Value* root = json_value_init_object();
  JSON_Object* obj = json_value_get_object(root);

  json_object_set_number(obj, "humidity", data->humidity);
  json_object_set_number(obj, "temperature", data->temperature);
  json_object_set_number(obj, "pressure", data->pressure);
  json_object_set_number(obj, "mag_x", data->mag_x);
  json_object_set_number(obj, "mag_y", data->mag_y);
  json_object_set_number(obj, "mag_z", data->mag_z);
  json_object_set_number(obj, "gyro_x", data->gyro_x);
  json_object_set_number(obj, "gyro_y", data->gyro_y);
  json_object_set_number(obj, "gyro_z", data->gyro_z);
  json_object_set_number(obj, "accel_x", data->accel_x);
  json_object_set_number(obj, "accel_y", data->accel_y);
  json_object_set_number(obj, "accel_z", data->accel_z);

  char* serialized = json_serialize_to_string_pretty(root);
  int res = snprintf(out_buf, buf_len, "%s", serialized);

  json_value_free(root);
  json_free_serialized_string(serialized);

  return res;
}