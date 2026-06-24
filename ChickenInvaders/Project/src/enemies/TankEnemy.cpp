#include "enemies/TankEnemy.hpp"
#include "AssetManager.hpp"
#include <algorithm>

TankEnemy::TankEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier)
    : Enemy(x, y, 54.f, 54.f, 5 + level * 2,
            32.f * speedMultiplier,
            std::max(0.45f, 1.55f / shootMultiplier),
            150 + level * 30, 20.f, 0.6f, sf::Color(150, 80, 200)) {
    if (AssetManager::has("enemy_tank")) setTexture(AssetManager::get("enemy_tank"));
}
