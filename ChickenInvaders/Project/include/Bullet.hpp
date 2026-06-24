#pragma once
#include "Entity.hpp"

class Bullet : public Entity {
public:
    Bullet(float x, float y, float vx, float vy, bool isEnemy);
    void update(float dt) override;
    float vx;
    float vy;
    bool isEnemy;
};
