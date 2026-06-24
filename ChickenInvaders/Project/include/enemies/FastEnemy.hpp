#pragma once
#include "enemies/Enemy.hpp"
class FastEnemy : public Enemy {
public:
    FastEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier);
};
