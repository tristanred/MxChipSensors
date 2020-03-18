#include "DataSender.h"

int generate_message(const struct SensorData* data, char* out_buf, size_t buf_len);

DataSender::DataSender(const SensorData* source, uint32_t updateTime)
{
    this->initialized = false;

    if(source == NULL)
    {
        return;
    }

    this->initialized = true;
    this->enabled = true;
    this->updateCounter = 0;
    this->timeBetweenUpdates = updateTime;
    this->sourceData = source;
    this->messagesSent = 0;
}

void DataSender::Update(uint32_t deltaTime)
{
    if(this->initialized == false || this->enabled == false)
    {
        return;
    }

    this->updateCounter += deltaTime;

    if(this->updateCounter >= timeBetweenUpdates)
    {
        this->updateCounter = 0;
        this->messagesSent++;

        char msg[1024];
        int res = generate_message(this->sourceData, msg, 1024);

        if(res > 0)
        {
            this->messagesSent++;

            EVENT_INSTANCE* message = DevKitMQTTClient_Event_Generate(msg, MESSAGE);
            DevKitMQTTClient_SendEventInstance(message);

            char msg_count_buf[32];
            sprintf(msg_count_buf, "Msg sent : %lu", this->messagesSent);
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
}

int generate_message(const struct SensorData* data, char* out_buf, size_t buf_len)
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
