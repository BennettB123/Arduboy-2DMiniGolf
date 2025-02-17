#pragma once

#include "FX/ArduboyFX.h"
#include "FX/fxdata.h"
#include "Map.h"

class MapManager
{
public:
    static const uint8_t NumMaps = 8;

    static Map LoadMap(uint8_t index)
    {
        Map map;

        // read Map from FX data
        FX::seekArrayElement<Map>(Maps, index);
        FX::readObject(map);
        FX::readEnd();

        map.name = mapNames[index];

        return map;
    }

    static uint8_t GetTotalPar()
    {
        uint8_t total = 0;
        Map map;

        for (uint8_t i = 0; i < NumMaps; i++)
        {
            // read Map from FX data
            FX::seekArrayElement<Map>(Maps, i);
            FX::readObject(map);
            FX::readEnd();

            total += map.par;
        }

        return total;
    }

private:
    static const char *mapNames[NumMaps];
};

const char *MapManager::mapNames[MapManager::NumMaps] = {
    "Squiggly Lane",
    "Round Rodeo",
    "The Diamond",
    "Treadmill Alley",
    "Haunted Hallway",
    "Options",
    "Plinko",
    "Ricochet"};