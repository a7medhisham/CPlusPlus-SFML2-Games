#include "enemies/Enemy.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cstdlib>
#include <cmath>

Enemy::Enemy(float x, float y, float w, float h, int hp, float speed,
    float shootInterval, int scoreValue, float amplitude,
    float frequency, sf::Color fallbackColor)
    : Entity(x, y, w, h, fallbackColor), hp(hp), maxHp(hp),
    shootInterval(std::max(0.25f, shootInterval)), speed(speed),
    scoreValue(scoreValue), amplitude(amplitude), frequency(frequency),
    baseX(x), baseY(y) {
    shootTimer = static_cast<float>(rand() % 100) / 100.f * this->shootInterval;
}

bool Enemy::updateEnemy(float dt) {
    moveTimer += dt;
    x = baseX + std::sin(moveTimer * frequency) * amplitude;
    baseY += speed * dt;
    y = baseY;
    if (y > Constants::WINDOW_H + 60.f) {
        alive = false;
        escaped = true;
    }
    shootTimer += dt;
    if (shootTimer >= shootInterval) {
        shootTimer = 0.f;
        return true;
    }
    return false;
}

void Enemy::update(float dt) { updateEnemy(dt); }

void Enemy::draw(sf::RenderWindow& window) {
    Entity::draw(window);
    const float ratio = std::max(0.f, static_cast<float>(hp) / static_cast<float>(maxHp));
    sf::RectangleShape barBackground({ w, 4.f });
    barBackground.setFillColor(sf::Color(80, 0, 0));
    barBackground.setPosition(x - w / 2.f, y - h / 2.f - 8.f);
    window.draw(barBackground);
    sf::RectangleShape barForeground({ w * ratio, 4.f });
    barForeground.setFillColor(sf::Color(0, 220, 80));
    barForeground.setPosition(x - w / 2.f, y - h / 2.f - 8.f);
    window.draw(barForeground);
}

std::vector<std::unique_ptr<Bullet>> Enemy::shoot() {
    std::vector<std::unique_ptr<Bullet>> bullets;
    bullets.push_back(std::make_unique<Bullet>(x, y + h / 2.f, 0.f, Constants::ENEMY_BULLET_SPEED_BASE, true));
    return bullets;
}

bool Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        alive = false;
        return true;
    }
    return false;
}

int Enemy::getScoreValue() const { return scoreValue; }
int Enemy::getHp() const { return hp; }
int Enemy::getMaxHp() const { return maxHp; }
bool Enemy::isBoss() const { return false; }
