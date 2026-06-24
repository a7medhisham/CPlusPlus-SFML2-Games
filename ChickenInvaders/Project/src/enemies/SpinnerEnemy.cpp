#include "enemies/SpinnerEnemy.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cmath>

SpinnerEnemy::SpinnerEnemy(float x, float y, int level, float speedMultiplier, float shootMultiplier)
    : Enemy(x, y, 46.f, 46.f, 3 + level,
            62.f * speedMultiplier,
            std::max(0.35f, 1.05f / shootMultiplier),
            200 + level * 40, 60.f, 1.6f, sf::Color(255, 100, 150)) {
    if (AssetManager::has("enemy_spinner")) setTexture(AssetManager::get("enemy_spinner"));
}

std::vector<std::unique_ptr<Bullet>> SpinnerEnemy::shoot() {
    std::vector<std::unique_ptr<Bullet>> bullets;
    int count = 4 + static_cast<int>(spinAngle / 90.f);
    count = std::min(count, 8);
    for (int i = 0; i < count; ++i) {
        const float angle = (spinAngle + i * 360.f / count) * Constants::PI / 180.f;
        const float bulletSpeed = Constants::ENEMY_BULLET_SPEED_BASE;
        bullets.push_back(std::make_unique<Bullet>(x, y, std::sin(angle) * bulletSpeed, std::cos(angle) * bulletSpeed, true));
    }
    spinAngle += 45.f;
    if (spinAngle >= 360.f) spinAngle -= 360.f;
    return bullets;
}
