#include "PowerUp.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cstdlib>

PowerUp::PowerUp(float x, float y) : x(x), y(y) {
    type = static_cast<Type>(rand() % 3);
}

void PowerUp::update(float dt) {
    y += vy * dt;
    if (y > Constants::WINDOW_H + 30.f) alive = false;
}

void PowerUp::draw(sf::RenderWindow& window) {
    const char* textureNames[] = {"power_multi", "power_life", "power_bomb"};
    const int index = static_cast<int>(type);
    if (AssetManager::has(textureNames[index])) {
        sf::Sprite sprite;
        sprite.setTexture(AssetManager::get(textureNames[index]));
        const sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sprite.setScale(30.f / bounds.width, 30.f / bounds.height);
        sprite.setPosition(x, y);
        window.draw(sprite);
        return;
    }
    sf::CircleShape circle(14.f);
    sf::Color colors[] = {sf::Color(255, 220, 0), sf::Color(0, 255, 100), sf::Color(255, 60, 200)};
    circle.setFillColor(colors[index]);
    circle.setOutlineColor(sf::Color::White);
    circle.setOutlineThickness(2.f);
    circle.setPosition(x - 14.f, y - 14.f);
    window.draw(circle);
}

sf::FloatRect PowerUp::getBounds() const { return {x - 15.f, y - 15.f, 30.f, 30.f}; }
PowerUp::Type PowerUp::getType() const { return type; }
