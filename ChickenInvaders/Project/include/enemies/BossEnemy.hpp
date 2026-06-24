#pragma once
#include "enemies/Enemy.hpp"
class BossEnemy : public Enemy {
public:
    BossEnemy(float x, float y, int level);
    bool updateEnemy(float dt) override;
    std::vector<std::unique_ptr<Bullet>> shoot() override;
    bool isBoss() const override;
private:
    int phase = 1;
    float phaseTimer = 0.f;
    bool movingRight = true;
};
