#pragma once
#include "enemies/Enemy.hpp"
class TankEnemy : public Enemy {
public:
    TankEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier);
};
