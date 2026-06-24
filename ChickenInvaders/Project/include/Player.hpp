#pragma once
#include "Bullet.hpp"
#include "Entity.hpp"
#include <memory>
#include <vector>

class Player : public Entity {
public:
    Player();
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    std::vector<std::unique_ptr<Bullet>> shoot();
    bool hit();
    void loseLifeFromEscape();
    void setLevel(int newLevel);
    int getLevel() const;
    int lives = 3;
    int score = 0;
    int eggs = 0;
    bool multiShot = false;
private:
    float shootTimer = 0.f;
    float invincibleTimer = 0.f;
    int level = 1;
};
