#pragma once
#include <SFML/Graphics.hpp>

class Particle {
public:
    Particle(float x, float y, sf::Color color);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool alive = true;
private:
    float x;
    float y;
    float vx;
    float vy;
    float life;
    float maxLife;
    float size;
    sf::Color color;
};
