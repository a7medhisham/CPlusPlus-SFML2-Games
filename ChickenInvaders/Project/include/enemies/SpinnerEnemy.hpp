#pragma once
#include "enemies/Enemy.hpp"
class SpinnerEnemy : public Enemy {
public:
    SpinnerEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier);
    std::vector<std::unique_ptr<Bullet>> shoot() override;
private:
    float spinAngle = 0.f;
};
