#include "Starfield.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cstdlib>

Starfield::Starfield(int count) {
    stars.reserve(count);
    for (int i = 0; i < count; ++i) {
        stars.push_back({static_cast<float>(rand() % Constants::WINDOW_W),
                         static_cast<float>(rand() % Constants::WINDOW_H),
                         20.f + static_cast<float>(rand() % 60),
                         1.f + static_cast<float>(rand() % 3) * 0.5f});
    }
}

void Starfield::update(float dt) {
    for (auto& star : stars) {
        star.y += star.speed * dt;
        if (star.y > Constants::WINDOW_H) {
            star.y = 0.f;
            star.x = static_cast<float>(rand() % Constants::WINDOW_W);
        }
    }
}

void Starfield::draw(sf::RenderWindow& window) {
    for (auto& star : stars) {
        const sf::Uint8 brightness = 100 + static_cast<sf::Uint8>(star.speed * 2.f);
        if (AssetManager::has("star")) {
            sf::Sprite sprite;
            sprite.setTexture(AssetManager::get("star"));
            const sf::FloatRect bounds = sprite.getLocalBounds();
            sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            const float targetSize = star.size * 3.f;
            sprite.setScale(targetSize / bounds.width, targetSize / bounds.height);
            sprite.setPosition(star.x, star.y);
            sprite.setColor(sf::Color(brightness, brightness, brightness, 180));
            window.draw(sprite);
            continue;
        }
        sf::CircleShape circle(star.size);
        circle.setFillColor(sf::Color(brightness, brightness, brightness, 180));
        circle.setPosition(star.x, star.y);
        window.draw(circle);
    }
}
