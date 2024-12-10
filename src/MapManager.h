#pragma once

#include "FX/ArduboyFX.h"
#include "FX/fxdata.h"
#include "Map.h"

class MapManager
{
public:
    static const uint8_t NumMaps = 2;

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

private:
    static const char* mapNames[NumMaps];
};

const char *MapManager::mapNames[MapManager::NumMaps] = {
    "Squiggly Lane",
    "Round Rodeo"};