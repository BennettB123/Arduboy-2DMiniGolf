// initialization for ArduboyG library
#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW

#include "ArduboyG.h"
#include "Game.h"

ArduboyG arduboy;
Game game(arduboy);
unsigned long previousTime = 0;

void setup()
{
    arduboy.begin();
    // arduboy.setFrameRate(60);
    arduboy.initRandomSeed();

    arduboy.startGray();

    previousTime = millis();
}

void loop()
{
    arduboy.waitForNextPlane();
    if (arduboy.needsUpdate())
    {
        unsigned long currentTime = millis();
        unsigned long timeDelta = currentTime - previousTime;
        previousTime = currentTime;
        float secondsDelta = timeDelta * (1.0f / 1000.0f);

        arduboy.pollButtons();
        game.Tick(secondsDelta);
    }

    game.Display();
}
