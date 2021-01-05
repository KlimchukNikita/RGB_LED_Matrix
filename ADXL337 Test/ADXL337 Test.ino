#define PIN_X A0
#define PIN_Y A1
#define PIN_Z A2

// 3 (±3g) for ADXL337
// 200 (±200g) for ADXL377
int scale = 3;

// True if using a 5V microcontroller
// False if using a 3.3V microcontroller
boolean micro_is_5V = true;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    int rawX = analogRead(PIN_X);
    int rawY = analogRead(PIN_Y);
    int rawZ = analogRead(PIN_Z);

    float scaledX, scaledY, scaledZ;

    // 3.3/5 * 1023 =~ 675
    if (micro_is_5V)
    {
        scaledX = mapf(rawX, 0, 675, -scale, scale);
        scaledY = mapf(rawY, 0, 675, -scale, scale);
        scaledZ = mapf(rawZ, 0, 675, -scale, scale);
    }

    else
    {
        scaledX = mapf(rawX, 0, 1023, -scale, scale);
        scaledY = mapf(rawY, 0, 1023, -scale, scale);
        scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
    }

    Serial.print("X: "); Serial.println(rawX);
    Serial.print("Y: "); Serial.println(rawY);
    Serial.print("Z: "); Serial.println(rawZ);
    Serial.println();

    Serial.print("X: "); Serial.print(scaledX); Serial.println(" g");
    Serial.print("Y: "); Serial.print(scaledY); Serial.println(" g");
    Serial.print("Z: "); Serial.print(scaledZ); Serial.println(" g");
    Serial.println();

    delay(2000);
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
