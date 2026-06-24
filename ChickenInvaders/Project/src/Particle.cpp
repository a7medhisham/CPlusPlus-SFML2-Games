#include "Particle.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cmath>
#include <cstdlib>

Particle::Particle(float x, float y, sf::Color color) : x(x), y(y), color(color) {
    const float angle = static_cast<float>(rand() % 360) * Constants::PI / 180.f;
    const float speed = 50.f + static_cast<float>(rand() % 200);
    vx = std::cos(angle) * speed;
    vy = std::sin(angle) * speed;
    maxLife = life = 0.4f + static_cast<float>(rand() % 40) / 100.f;
    size = 3.f + static_cast<float>(rand() % 5);
}

void Particle::update(float dt) {
    x += vx * dt;
    y += vy * dt;
    life -= dt;
    if (life <= 0.f) alive = false;
}

void Particle::draw(sf::RenderWindow& window) {
    const float ratio = life / maxLife;
    if (AssetManager::has("star")) {
        sf::Sprite sprite;
        sprite.setTexture(AssetManager::get("star"));
        const sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        const float targetSize = size * ratio * 3.f;
        sprite.setScale(targetSize / bounds.width, targetSize / bounds.height);
        sprite.setPosition(x, y);
        sprite.setColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(255 * ratio)));
        window.draw(sprite);
        return;
    }
    sf::CircleShape circle(size * ratio);
    circle.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(255 * ratio)));
    circle.setPosition(x - size * ratio, y - size * ratio);
    window.draw(circle);
}
