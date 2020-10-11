#include <FastLED.h>

#define NUM_LEDS  256
#define DATA_PIN  3
#define BRIGHTNESS  8

#define pinX  A2
#define pinY  A1
#define swPin  2

int snake[256];
int snakeSize = 2;
int snakeSpeed = 500;

int row;
int col;

int lastDirection = 135;
int i, newDirection, OlddX = 1, OlddY, f;

int red, green, blue, fred, fgreen, fblue;
CRGB leds[NUM_LEDS];

void setup()
{
    red = random(0, 255);
    green = random(0, 255);
    blue = random(0, 255);

    fred = random(127, 255);
    fgreen = random(127, 255);
    fblue = random(127, 255);

    Serial.begin(9600);

    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);
    pinMode(swPin, INPUT);

    digitalWrite(swPin, HIGH);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    for (i=0; i<=255; i++)
    {
        snake[i] = 0;
    }

    for (i=0; i<=snakeSize; i++)
    {
        snake[i] = lastDirection+i;
    }

    f = random(0, 255);
    FastLED.show();
}

int Snakedirection (int last, int dX, int dY)
{
    dX = map(dX, 0, 1000, -1, 1);
    dY = map(dY, 0, 1000, -1, 1);

    if (dX == 0 && dY == 0 && OlddX != dX)
    {
        dX = OlddX;
    }

    if (dY == 0 && dX == 0 && OlddY != dY)
    {
        dY = OlddY;
    }

    int newDirection = last;

    if (dX != 0)
    {
        if (row&1)
        {
            if (col == 0 && dX == 1)
            {
                newDirection = last -15;
            }

            else if (col == 15 && dX == -1)
            {
                newDirection = last +15;
            }

            else newDirection = last + dX;
        }

        else
        {
            if (col == 0 && dX == 1)
            {
                newDirection = last +15;
            }

            else if (col == 15 && dX == -1)
            {
                newDirection = last -15;
            }

            else newDirection = last - dX;
        }
    }

    if (dY < 0)
    {
        if (row == 15 && dY == -1)
        {
            newDirection = col;
        }

        else if (row&1)
        {
            newDirection = last + (col*2)+1;
        }

        else
        {
            newDirection = last + (16-col-1)+(16-col);
        }
    }

    if (dY > 0)
    {
        if (row == 0 && dY == 1)
        {
            newDirection = 255 - col;
        }

        else if (row&1)
        {
            newDirection = last - (last - 16*row)-(16-col);
        }

        else
        {
            newDirection = last - (col*2)-1;
        }
    }

    OlddX = dX;
    OlddY = dY;

    return newDirection;
}

int snakeMove (int snakeDirection)
{
    for (i=0; i<=255; i++)
    {
        if (snake[i] == snakeDirection)
        {
            death();
        }
    }

    FastLED.clear();

    for (i=snakeSize; i>=1; i--)
    {
        snake[i] = snake[i-1];
    }

    snake[0] = snakeDirection;

    for (i=0; i<=255; i++)
    {
        if (snake[i])
        {
            leds[snake[i]].setRGB(red, green, blue);
        }
    }

    FastLED.show();

    row = (int)(snakeDirection/16);

    if (row&1)
    {
        col = (row+1)*16 - snakeDirection - 1;
    }

    else
    {
        col = snakeDirection - row*16;
    }

    return snakeDirection;
}

void food (int eaten)
{
    if (eaten == f)
    {
        snakeSize++;
        f = random(0, 255);

        red = fred;
        green = fgreen;
        blue = fblue;

        fred = random(0, 255);
        fgreen = random(0, 255);
        fblue = random(0, 255);

        snakeSpeed = snakeSpeed / 1.1;
    }

    else
    {
        leds[f].setRGB(fred, fgreen, fblue);
        FastLED.show();
    }
}

void death()
{
    snakeSize = 2;
    snakeSpeed = 500;
    red = 255;
    green = 0;
    blue = 0;
}

void color (boolean sw)
{
    if (!sw)
    {
        red = random(0,255);
        green = random(0,255);
        blue = random(0,255);
    }
}

void loop()
{
    color (digitalRead(swPin));

    newDirection = Snakedirection(lastDirection, analogRead(pinX), analogRead(pinY));
    lastDirection = snakeMove(newDirection);

    food(newDirection);
    delay(snakeSpeed);
}
