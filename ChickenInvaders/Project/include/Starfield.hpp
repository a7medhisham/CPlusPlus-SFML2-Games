#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Starfield {
public:
    explicit Starfield(int count = 120);
    void update(float dt);
    void draw(sf::RenderWindow& window);
private:
    struct Star { float x; float y; float speed; float size; };
    std::vector<Star> stars;
};
