#pragma once
#include "Bullet.hpp"
#include "Entity.hpp"
#include <memory>
#include <vector>

class Enemy : public Entity {
public:
    Enemy(float x, float y, float w, float h, int hp, float speed,
          float shootInterval, int scoreValue, float amplitude,
          float frequency, sf::Color fallbackColor);
    virtual bool updateEnemy(float dt);
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    virtual std::vector<std::unique_ptr<Bullet>> shoot();
    bool takeDamage(int damage);
    int getScoreValue() const;
    int getHp() const;
    int getMaxHp() const;
    virtual bool isBoss() const;
protected:
    int hp;
    int maxHp;
    float shootTimer = 0.f;
    float shootInterval;
    float speed;
    int scoreValue;
    float moveTimer = 0.f;
    float amplitude;
    float frequency;
    float baseX;
    float baseY;
};
