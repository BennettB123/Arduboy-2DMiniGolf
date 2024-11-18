#include "Game.h"
#include <Arduboy2.h>

Arduboy2 arduboy;
Game game(arduboy);
unsigned long previousTime = 0;

void setup()
{
    arduboy.begin();
    arduboy.setFrameRate(60);
    arduboy.initRandomSeed();

    previousTime = millis();
}

void loop()
{
    while (!arduboy.nextFrameDEV())
        return;

    arduboy.clear();
    arduboy.pollButtons();

    unsigned long currentTime = millis();
    unsigned long timeDelta = currentTime - previousTime;
    previousTime = currentTime;
    float secondsDelta = timeDelta * (1.0f / 1000.0f);

    game.Tick(secondsDelta);
    game.Display();

    arduboy.display();
}
