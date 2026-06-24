#include "enemies/FastEnemy.hpp"
#include "AssetManager.hpp"
#include <algorithm>

FastEnemy::FastEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier)
    : Enemy(x, y, 38.f, 38.f, 1,
            (105.f + level * 18.f) * speedMultiplier,
            std::max(0.65f, 2.4f / shootMultiplier),
            80 + level * 20, 80.f, 3.0f, sf::Color(50, 200, 255)) {
    if (AssetManager::has("enemy_fast")) setTexture(AssetManager::get("enemy_fast"));
}
