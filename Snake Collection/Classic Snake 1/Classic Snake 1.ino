#include <Adafruit_NeoPixel.h>

#define BUZZER_PIN  8

#define MATRIX_B_PIN  6
#define MATRIX_E_PIN  9
#define MATRIX_C_PIN  4
#define MATRIX_F_PIN  10

#define LED_COUNT  16

#define ANALOG_PIN_FOR_RND  A3

#define BRIGHT  10

#define X  A5
#define Y  A4
#define Z  2

Adafruit_NeoPixel matrix [] =
{
    Adafruit_NeoPixel(LED_COUNT, MATRIX_B_PIN, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_COUNT, MATRIX_E_PIN, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_COUNT, MATRIX_C_PIN, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_COUNT, MATRIX_F_PIN, NEO_GRB + NEO_KHZ800)
};

enum
{
    NOTOUCH,
    RIGHT,
    LEFT,
    DOWN,
    UP
};

struct Light
{
    uint32_t R = 0xff0000;
    uint32_t G = 0x00ff00;
    uint32_t B = 0x0000ff;
};

struct Store
{
    int value;
    bool flag;
};

struct Snakes
{
    int x;
    int y;
    bool flagX;
    bool flagY;
};

struct Mouses
{
    int x;
    int y;
};

Light light;
Store store;
Snakes snake;
Mouses mouse;

unsigned long timeOfMovement = 0;

int movement = 500;
int player, playerOld;
int tailX[64], tailY[64];

void setup()
{
    pinMode(BUZZER_PIN, OUTPUT);

    randomSeed(analogRead(ANALOG_PIN_FOR_RND));

    for (int i = 0; i < sizeof(matrix) / sizeof(Adafruit_NeoPixel); i++)
    {
        matrix[i].begin();
        matrix[i].setBrightness(10);
    }

    Serial.begin(9600);

    draw_clear ();
}

void loop()
{
    at_the_start();

    while (store.flag)
    {
        player_move ('x');
        player_move ('y');

        if (snake.x == mouse.x && snake.y == mouse.y)
        {
            for (int i = 0; i < 64; i++)
            {
                if (mouse.x == tailX[i] && mouse.y == tailY[i])
                {
                    mouse.x = random(0, 8);
                    mouse.y = random(0, 8);

                    i = 0;
                }
            }

            store.value++;

            tone(BUZZER_PIN, 440, 100);
        }

        if (millis() -  timeOfMovement > movement)
        {
            draw_clear ();
            draw_point (mouse.x, mouse.y, light.R);
            snake_draw ();

            if (snake.x < 0 || snake.x >= 8 || snake.y < 0 || snake.y >= 8)
            {
                game_over();
            }

            for (int i = 1; i <= store.value; i++)
            {
                if (snake.x == tailX[i] && snake.y == tailY[i])
                {
                    game_over();
                }
            }

            timeOfMovement =  millis();
        }
    }
}

void at_the_start()
{
    draw_clear ();

    store = {0, false};

    player = NOTOUCH;
    playerOld = NOTOUCH;

    player_move ('z');

    snake = {random(0, 8), random(0, 8), true, true};
    mouse = {random(0, 8), random(0, 8)};

    draw_point (mouse.x, mouse.y, light.R);
    delay (movement);
}

void game_over()
{
    tone(BUZZER_PIN, 261, 200);

    draw_clear ();
    draw_pouring (light.B);

    Serial.print ("Your store: ");
    Serial.println (store.value);

    store.flag = false;

    randomSeed(analogRead(ANALOG_PIN_FOR_RND));
    delay (movement);
}

void player_move (char coordinate)
{
    switch (coordinate)
    {
        case 'x':
        {
            int x;

            x = analogRead(X);
            Serial.println(x);

            if (x < 350 && snake.flagX)
            {
                player = RIGHT;
                snake.flagX = false;
            }

            if (x > 850 && snake.flagX)
            {
                player = LEFT;
                snake.flagX = false;
            }

            if (350 < x && x < 850)
            {
                snake.flagX = true;
            }

            break;
        }

        case 'y':
        {
            int y;

            y = analogRead(Y);
            Serial.println(y);

            if (y < 350 && snake.flagY)
            {
                player = UP;
                snake.flagY = false;
            }

            if (y > 850 && snake.flagY)
            {
                player = DOWN;
                snake.flagY = false;
            }

            if (350 < y && y < 850)
            {
                snake.flagY = true;
            }

            break;
        }

        case 'z':
        {
            int z;

            z = digitalRead(Z);
            Serial.println(z);

            if (z == 1 && store.flag == false)
            {
                store.flag = true;
            }

            break;
        }
    }
}

void snake_draw ()
{
    switch (player)
    {
        case RIGHT:
        {
            if (playerOld != LEFT)
            {
                snake.x++;
            }

            break;
        }

        case LEFT:
        {
            if (playerOld != RIGHT)
            {
                snake.x--;
            }

            break;
        }

        case UP:
        {
            if (playerOld != DOWN)
            {
                snake.y++;
            }

            break;
        }

        case DOWN:
        {
            if (playerOld != UP)
            {
                snake.y--;
            }

            break;
        }
    }

    playerOld = player;

    tail_create ();

    for (int i = 0; i <= store.value; i++)
    {
        draw_point (tailX[i], tailY[i], light.G);
    }
}

void tail_create ()
{
    int bufferX = tailX[0];
    int bufferY = tailY[0];
    int bufferX2, bufferY2;

    tailX[0] = snake.x;
    tailY[0] = snake.y;

    for (int i = 1; i <= store.value; i++)
    {
        bufferX2 = tailX[i];
        bufferY2 = tailY[i];

        tailX[i] = bufferX;
        tailY[i] = bufferY;

        bufferX = bufferX2;
        bufferY = bufferY2;
    }
}

int nHelper (int x, int y)
{
    return y / 4 + ((x / 4) * 2);
}

int mHelper(int x, int y)
{
    switch (nHelper (x, y))
    {
        case 0:
        {
            return {y * 4 + x};
            break;
        }

        case 1:
        {
            return {abs (y - 8) * 4 - x - 1};
            break;
        }

        case 2:
        {
            return {y % 4 * 4 + x % 4};
            break;
        }

        case 3:
        {
            return {abs (y - 8) * 4 - x % 4 - 1};
            break;
        }
    }
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t RGB)
{
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;

    int error = deltaX - deltaY;

    matrix[nHelper(x2, y2)].setPixelColor(mHelper(x2, y2), RGB);

    while (x1 != x2 || y1 != y2)
    {
        matrix[nHelper(x1, y1)].setPixelColor(mHelper(x1, y1), RGB);

        const int error2 = error * 2;

        if (error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }

        if (error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }

    for (int i = 0; i < sizeof(matrix) / sizeof(Adafruit_NeoPixel); i++)
    {
        matrix[i].show();
    }
}

void draw_pouring (uint32_t RGB)
{
    for (int i = 0; i < sizeof(matrix) / sizeof(Adafruit_NeoPixel); i++)
    {
        for (int j = 0; j < LED_COUNT; j++)
        {
            matrix[i].setPixelColor(j, RGB);
            matrix[i].show();
        }
    }
}

void draw_point (int x, int y, uint32_t RGB)
{
    matrix[nHelper(x, y)].setPixelColor(mHelper(x, y), RGB);
    matrix[nHelper(x, y)].show();
}

void draw_clear ()
{
    for (int i = 0; i < sizeof(matrix) / sizeof(Adafruit_NeoPixel); i++)
    {
        matrix[i].clear();
        matrix[i].show();
    }
}
