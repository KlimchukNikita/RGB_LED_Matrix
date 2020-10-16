#include <Adafruit_NeoPixel.h>

#define PIN_LED_MATRIX   2

#define PIN_BUTTON_UP    4
#define PIN_BUTTON_DOWN  5
#define PIN_BUTTON_LEFT  6
#define PIN_BUTTON_RIGHT 7

#define X_AXIS A2
#define Y_AXIS A3

#define DEBOUNCE_TIME 300

#define X_MAX 8
#define Y_MAX 8

#define GAME_DELAY 400

#define LED_TYPE_SNAKE 1
#define LED_TYPE_OFF   2
#define LED_TYPE_FOOD  3
#define LED_TYPE_BLOOD 4

#define DIRECTION_NONE 0

#define DIRECTION_UP    1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3
#define DIRECTION_RIGHT 4

#define GAME_STATE_RUNNING 1
#define GAME_STATE_END     2
#define GAME_STATE_INIT    3

#define MAX_TAIL_LENGTH X_MAX * Y_MAX
#define MIN_TAIL_LENGTH 3

struct Coords
{
    int x;
    int y;
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(X_MAX*Y_MAX, PIN_LED_MATRIX, NEO_GRB + NEO_KHZ800);

byte incomingByte = 0;
byte userDirection;
byte gameState;

Coords head;
Coords tail[MAX_TAIL_LENGTH];
Coords food;

unsigned long lastDrawUpdate = 0;
unsigned long lastButtonClick;
unsigned int wormLength = 0;

void setup()
{
    pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_LEFT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_RIGHT, INPUT_PULLUP);

    Serial.begin(9600);

    pixels.begin();
    resetLEDs();

    gameState = GAME_STATE_END;
}

void loop()
{
    switch (gameState)
    {
        case GAME_STATE_INIT:
        initGame();
        break;

        case GAME_STATE_RUNNING:
        checkButtonPressed();
        updateGame();
        break;

        case GAME_STATE_END:
        checkButtonPressed();
        break;
    }

    int rotationX;
    rotationX = analogRead (X_AXIS);

    if ((rotationX >= 0) && (rotationX < 341))
    {
        userDirection = DIRECTION_UP;
    }

    if ((rotationX >= 682) && (rotationX < 1023))
    {
        userDirection = DIRECTION_DOWN;
    }

    int rotationY;
    rotationY = analogRead (Y_AXIS);

    if ((rotationY >= 0) && (rotationY < 341))
    {
        userDirection = DIRECTION_LEFT;
    }

    if ((rotationY >= 682) && (rotationY < 1023))
    {
        userDirection = DIRECTION_RIGHT;
    }
}

void resetLEDs()
{
    for (byte i=0; i<X_MAX*Y_MAX; i++)
    {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }

    pixels.show();
}

void initGame()
{
    resetLEDs();

    head.x = 0;
    head.y = 0;

    food.x = -1;
    food.y = -1;

    wormLength = MIN_TAIL_LENGTH;
    userDirection = DIRECTION_LEFT;
    lastButtonClick = millis();

    for (byte i=0; i<MAX_TAIL_LENGTH; i++)
    {
        tail[i].x = -1;
        tail[i].y = -1;
    }

    updateFood();
    gameState = GAME_STATE_RUNNING;
}

void updateGame()
{
    if ((millis() - lastDrawUpdate) > GAME_DELAY)
    {
        toggleLed(tail[wormLength-1].x, tail[wormLength-1].y, LED_TYPE_OFF);

        switch (userDirection)
        {
            case DIRECTION_RIGHT:

            if (head.x > 0)
            {
                head.x--;
            }

            break;

            case DIRECTION_LEFT:

            if (head.x < X_MAX-1)
            {
                head.x++;
            }

            break;

            case DIRECTION_DOWN:

            if (head.y > 0)
            {
                head.y--;
            }

            break;

            case DIRECTION_UP:

            if (head.y < Y_MAX-1)
            {
                head.y++;
            }

            break;
        }

        if (isCollision() == true)
        {
            endGame();
            return;
        }

        updateTail();

        if (head.x == food.x && head.y == food.y)
        {
            if (wormLength < MAX_TAIL_LENGTH)
            {
                wormLength++;
            }

            updateFood();
        }

        lastDrawUpdate = millis();
        pixels.show();
    }
}

void endGame()
{
    gameState = GAME_STATE_END;
    toggleLed(head.x, head.y, LED_TYPE_BLOOD);
    pixels.show();
}

void updateTail()
{
    for (byte i=wormLength-1; i>0; i--)
    {
        tail[i].x = tail[i-1].x;
        tail[i].y = tail[i-1].y;
    }

    tail[0].x = head.x;
    tail[0].y = head.y;

    for (byte i=0; i<wormLength; i++)
    {
        if (tail[i].x > -1)
        {
            toggleLed(tail[i].x, tail[i].y, LED_TYPE_SNAKE);
        }
    }
}

void updateFood()
{
    bool found = true;

    do
    {
        found = true;

        food.x = random(0, X_MAX);
        food.y = random(0, Y_MAX);

        for (byte i=0; i<wormLength; i++)
        {
            if (tail[i].x == food.x && tail[i].y == food.y)
            {
                found = false;
            }
        }
    }

    while (found == false);
    toggleLed(food.x, food.y, LED_TYPE_FOOD);
}

bool isCollision()
{
    if (head.x < 0 || head.x >= X_MAX)
    {
        return true;
    }

    if (head.y < 0 || head.y >= Y_MAX)
    {
        return true;
    }

    for (byte i=1; i<wormLength; i++)
    {
        if (tail[i].x == head.x && tail[i].y == head.y)
        {
            return true;
        }
    }

    return false;
}

void checkButtonPressed()
{
    if (millis() - lastButtonClick < DEBOUNCE_TIME)
    {
        return;
    }

    if (digitalRead(PIN_BUTTON_UP) == LOW)
    {
        if (gameState == GAME_STATE_RUNNING)
        {
            userDirection = DIRECTION_UP;
        }

        lastButtonClick = millis();
    }

    else if (digitalRead(PIN_BUTTON_DOWN) == LOW)
    {
        if (gameState == GAME_STATE_RUNNING)
        {
            userDirection = DIRECTION_DOWN;
        }

        lastButtonClick = millis();
    }

    else if (digitalRead(PIN_BUTTON_RIGHT) == LOW)
    {
        if (gameState == GAME_STATE_RUNNING)
        {
            userDirection = DIRECTION_RIGHT;
        }

        lastButtonClick = millis();
    }

    else if (digitalRead(PIN_BUTTON_LEFT) == LOW)
    {
        if (gameState == GAME_STATE_RUNNING)
        {
            userDirection = DIRECTION_LEFT;
        }

        else if (gameState == GAME_STATE_END)
        {
            gameState = GAME_STATE_INIT;
        }

        lastButtonClick = millis();
    }
}

void toggleLed(byte x, byte y, byte type)
{
    byte ledIndex = y * X_MAX + x;
    uint32_t color;

    switch (type)
    {
        case LED_TYPE_SNAKE:
        color = pixels.Color(0, 10, 10);
        break;

        case LED_TYPE_OFF:
        color = pixels.Color(0, 0, 0);
        break;

        case LED_TYPE_FOOD:
        color = pixels.Color(0, 15, 0);
        break;

        case LED_TYPE_BLOOD:
        color = pixels.Color(15, 0, 0);
        break;
    }

    pixels.setPixelColor(ledIndex, color);
}
