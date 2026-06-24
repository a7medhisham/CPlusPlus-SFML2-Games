#pragma once
#include "enemies/Enemy.hpp"
class NormalEnemy : public Enemy {
public:
    NormalEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier);
};
