#include "I2Cdev.h"
#include "MPU6050.h"

#define TO_DEG 57.29577951308232087679815481410517033f
#define T_OUT 20

MPU6050 accel;

float angle_ax;
long int t_next;

float clamp(float v, float minv, float maxv)
{
    if (v>maxv)
    return maxv;

    else if (v<minv)
    return minv;

    return v;
}

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
        float ay,gx;

        t_next = t + T_OUT;
        accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);

        // 1G = 4096
        ay = ay_raw / 4096.0;

        // -1G ... +1G
        ay = clamp(ay, -1.0, 1.0);

        if (ay >= 0)
        {
            angle_ax = 90 - TO_DEG*acos(ay);
        }

        else
        {
            angle_ax = TO_DEG*acos(-ay) - 90;
        }

        Serial.println(angle_ax);
    }
}
