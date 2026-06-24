#pragma once
#include <SFML/Graphics.hpp>

class PowerUp {
public:
    enum class Type { MultiShot = 0, Life = 1, Bomb = 2 };
    PowerUp(float x, float y);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    Type getType() const;
    bool alive = true;
    float x;
    float y;
private:
    float vy = 80.f;
    Type type;
};
