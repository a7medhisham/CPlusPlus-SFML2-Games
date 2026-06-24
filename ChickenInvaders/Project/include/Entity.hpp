#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    Entity(float x, float y, float w, float h, sf::Color fallbackColor);
    virtual ~Entity() = default;
    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool intersects(const Entity& other) const;
    void setTexture(const sf::Texture& texture);
    void setEntitySize(float newW, float newH);
    bool alive = true;
    bool escaped = false;
    float x;
    float y;
    float w;
    float h;
protected:
    sf::RectangleShape shape;
    sf::Sprite sprite;
    bool useSprite = false;
};
