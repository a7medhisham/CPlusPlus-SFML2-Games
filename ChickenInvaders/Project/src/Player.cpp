#include "Player.hpp"
#include "Constants.hpp"
#include <algorithm>

Player::Player()
    : Entity(Constants::WINDOW_W / 2.f, Constants::WINDOW_H - 70.f,
        44.f, 42.f, sf::Color(30, 180, 255)) {}

void Player::update(float dt) {
    shootTimer = std::max(0.f, shootTimer - dt);
    invincibleTimer = std::max(0.f, invincibleTimer - dt);
    x = std::max(w / 2.f, std::min(x, Constants::WINDOW_W - w / 2.f));
    y = std::max(h / 2.f, std::min(y, Constants::WINDOW_H - h / 2.f));
}

void Player::draw(sf::RenderWindow& window) {
    if (invincibleTimer > 0.f && static_cast<int>(invincibleTimer * 10.f) % 2 == 0) return;
    Entity::draw(window);
    if (!useSprite) {
        sf::ConvexShape nose;
        nose.setPointCount(3);
        nose.setPoint(0, { 0.f, -20.f });
        nose.setPoint(1, { -12.f, 0.f });
        nose.setPoint(2, { 12.f, 0.f });
        nose.setFillColor(sf::Color(0, 220, 255));
        nose.setPosition(x, y - h / 2.f);
        window.draw(nose);
    }
}

std::vector<std::unique_ptr<Bullet>> Player::shoot() {
    std::vector<std::unique_ptr<Bullet>> bullets;
    if (shootTimer > 0.f) return bullets;
    shootTimer = Constants::SHOOT_COOLDOWN;
    bullets.push_back(std::make_unique<Bullet>(x, y - h / 2.f, 0.f, -Constants::BULLET_SPEED, false));
    if (multiShot || level >= 3) {
        bullets.push_back(std::make_unique<Bullet>(x - 15.f, y - h / 2.f, -30.f, -Constants::BULLET_SPEED, false));
        bullets.push_back(std::make_unique<Bullet>(x + 15.f, y - h / 2.f, 30.f, -Constants::BULLET_SPEED, false));
    }
    if (level >= 7) {
        bullets.push_back(std::make_unique<Bullet>(x - 28.f, y - h / 4.f, -60.f, -Constants::BULLET_SPEED, false));
        bullets.push_back(std::make_unique<Bullet>(x + 28.f, y - h / 4.f, 60.f, -Constants::BULLET_SPEED, false));
    }
    return bullets;
}

bool Player::hit() {
    if (invincibleTimer > 0.f) return false;
    lives--;
    invincibleTimer = 2.5f;
    return true;
}

void Player::loseLifeFromEscape() { lives--; }
void Player::setLevel(int newLevel) { level = newLevel; }
int Player::getLevel() const { return level; }
