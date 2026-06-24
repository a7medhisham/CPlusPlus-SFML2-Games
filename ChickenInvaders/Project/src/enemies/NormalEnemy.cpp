#include "enemies/NormalEnemy.hpp"
#include "AssetManager.hpp"
#include <algorithm>

NormalEnemy::NormalEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier)
    : Enemy(x, y, 42.f, 42.f, 1 + level / 4,
            (45.f + level * 7.f) * speedMultiplier,
            std::max(0.55f, 2.0f / shootMultiplier),
            50 + level * 15, 45.f, 1.1f, sf::Color(255, 210, 70)) {
    if (AssetManager::has("enemy_normal")) setTexture(AssetManager::get("enemy_normal"));
}
