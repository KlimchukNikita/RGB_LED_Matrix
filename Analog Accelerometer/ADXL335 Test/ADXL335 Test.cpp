#define PIN_X A0
#define PIN_Y A1
#define PIN_Z A2

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int x = analogRead(PIN_X);
    int y = analogRead(PIN_Y);
    int z = analogRead(PIN_Z);
    
    Serial.print(x);
    Serial.print("  ");
    
    Serial.print(y);
    Serial.print("  ");
    
    Serial.print(z);
    Serial.println();
    
    delay(1000);
}
