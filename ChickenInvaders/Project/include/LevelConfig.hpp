#pragma once
#include <string>

struct LevelConfig {
    int level;
    int normalCount;
    int fastCount;
    int tankCount;
    int spinnerCount;
    bool hasBoss;
    float speedMultiplier;
    float shootMultiplier;
    std::string name;
};

LevelConfig getLevelConfig(int level);
