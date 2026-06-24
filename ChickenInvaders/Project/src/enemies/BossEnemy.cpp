#include "enemies/BossEnemy.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cmath>

BossEnemy::BossEnemy(float x, float y, int level)
    : Enemy(x, y, 110.f, 90.f, level >= 10 ? 130 : 80,
            level >= 10 ? 85.f : 65.f,
            level >= 10 ? 0.42f : 0.65f,
            level >= 10 ? 5000 : 2500, 0.f, 0.f, sf::Color(220, 40, 40)) {
    if (AssetManager::has("boss")) setTexture(AssetManager::get("boss"));
}

bool BossEnemy::updateEnemy(float dt) {
    phaseTimer += dt;
    if (hp <= maxHp / 2) phase = 2;
    const float currentSpeed = speed * (phase == 2 ? 1.75f : 1.f);
    if (movingRight) x += currentSpeed * dt;
    else x -= currentSpeed * dt;
    if (x > Constants::WINDOW_W - 90.f) movingRight = false;
    if (x < 90.f) movingRight = true;
    if (y < 190.f) y += 28.f * dt;
    shootTimer += dt;
    if (shootTimer >= shootInterval) {
        shootTimer = 0.f;
        return true;
    }
    return false;
}

std::vector<std::unique_ptr<Bullet>> BossEnemy::shoot() {
    std::vector<std::unique_ptr<Bullet>> bullets;
    const int spread = phase == 2 ? 2 : 1;
    for (int i = -spread; i <= spread; ++i) {
        const float angle = i * 18.f * Constants::PI / 180.f;
        const float bulletSpeed = Constants::ENEMY_BULLET_SPEED_BASE * (phase == 2 ? 1.35f : 1.2f);
        bullets.push_back(std::make_unique<Bullet>(x + i * 18.f, y + h / 2.f,
            std::sin(angle) * bulletSpeed, std::cos(angle) * bulletSpeed, true));
    }
    return bullets;
}

bool BossEnemy::isBoss() const { return true; }
