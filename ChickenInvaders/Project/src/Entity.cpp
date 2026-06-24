#include "Entity.hpp"

Entity::Entity(float x, float y, float w, float h, sf::Color fallbackColor)
    : x(x), y(y), w(w), h(h) {
    shape.setSize({w, h});
    shape.setFillColor(fallbackColor);
    shape.setOrigin(w / 2.f, h / 2.f);
    shape.setPosition(x, y);
}

void Entity::update(float) {}

void Entity::draw(sf::RenderWindow& window) {
    if (useSprite) {
        sprite.setPosition(x, y);
        window.draw(sprite);
        return;
    }
    shape.setPosition(x, y);
    window.draw(shape);
}

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(x - w / 2.f, y - h / 2.f, w, h);
}

bool Entity::intersects(const Entity& other) const {
    return getBounds().intersects(other.getBounds());
}

void Entity::setTexture(const sf::Texture& texture) {
    sprite.setTexture(texture, true);
    const sf::FloatRect bounds = sprite.getLocalBounds();
    if (bounds.width <= 0.f || bounds.height <= 0.f) {
        useSprite = false;
        return;
    }
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setScale(w / bounds.width, h / bounds.height);
    useSprite = true;
}

void Entity::setEntitySize(float newW, float newH) {
    w = newW;
    h = newH;
    shape.setSize({w, h});
    shape.setOrigin(w / 2.f, h / 2.f);
    if (useSprite) {
        const sf::FloatRect bounds = sprite.getLocalBounds();
        if (bounds.width > 0.f && bounds.height > 0.f) {
            sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            sprite.setScale(w / bounds.width, h / bounds.height);
        }
    }
}
