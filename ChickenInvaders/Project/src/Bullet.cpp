#include "Bullet.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"

Bullet::Bullet(float x, float y, float vx, float vy, bool isEnemy)
    : Entity(x, y, isEnemy ? 10.f : 8.f, isEnemy ? 16.f : 22.f,
             isEnemy ? sf::Color(255, 80, 80) : sf::Color(100, 220, 255)),
      vx(vx), vy(vy), isEnemy(isEnemy) {
    const std::string key = isEnemy ? "bullet_enemy" : "bullet_player";
    if (AssetManager::has(key)) setTexture(AssetManager::get(key));
}

void Bullet::update(float dt) {
    x += vx * dt;
    y += vy * dt;
    if (y < -30.f || y > Constants::WINDOW_H + 30.f ||
        x < -30.f || x > Constants::WINDOW_W + 30.f) {
        alive = false;
    }
}
