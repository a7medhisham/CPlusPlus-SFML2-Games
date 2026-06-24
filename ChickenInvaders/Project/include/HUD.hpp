#pragma once
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    HUD();
    void drawGameplay(sf::RenderWindow& window, const Player& player, int level,
                      int totalEnemies, int killedEnemies, int escapedEnemies);
    void drawCentered(sf::RenderWindow& window, const std::string& title,
                      const std::string& subtitle, const std::string& subtitle2 = "");
    bool isLoaded() const;
private:
    void drawText(sf::RenderWindow& window, const std::string& text,
                  float x, float y, unsigned int size, sf::Color color);
    sf::Font font;
    bool fontLoaded = false;
};
