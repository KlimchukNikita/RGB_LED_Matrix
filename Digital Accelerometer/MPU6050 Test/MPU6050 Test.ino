#include "I2Cdev.h"
#include "MPU6050.h"

#define T_OUT 20

MPU6050 accel;

unsigned long int t_next;

void setup()
{
    Serial.begin(9600);

    accel.initialize();
}

void loop()
{
    long int t = millis();

    if (t_next < t)
    {
        int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;

        t_next = t + T_OUT;

        accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);

        Serial.println(ay_raw);
    }
}
