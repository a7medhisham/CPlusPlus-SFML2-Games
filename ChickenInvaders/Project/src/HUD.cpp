#include "HUD.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <vector>

HUD::HUD() {
    const std::vector<std::string> paths = {
        "arial.ttf",
        "../arial.ttf",
        "../../arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/tahoma.ttf"
    };
    for (const auto& path : paths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
}

void HUD::drawText(sf::RenderWindow& window, const std::string& text,
                   float x, float y, unsigned int size, sf::Color color) {
    if (!fontLoaded) return;
    sf::Text drawableText;
    drawableText.setFont(font);
    drawableText.setString(text);
    drawableText.setCharacterSize(size);
    drawableText.setFillColor(color);
    drawableText.setPosition(x, y);
    window.draw(drawableText);
}

void HUD::drawGameplay(sf::RenderWindow& window, const Player& player, int level,
                       int totalEnemies, int killedEnemies, int escapedEnemies) {
    drawText(window, "SCORE: " + std::to_string(player.score), 10.f, 8.f, 20, sf::Color(255, 220, 50));
    drawText(window, "LEVEL: " + std::to_string(level), 10.f, 32.f, 18, sf::Color(100, 220, 255));
    drawText(
        window,
        "LIVES:",
        Constants::WINDOW_W - 180.f,
        8.f,
        18,
        sf::Color(255, 80, 80)
    );

    if (AssetManager::has("heart")) {
        for (int i = 0; i < player.lives; ++i) {
            sf::Sprite heart;
            heart.setTexture(AssetManager::get("heart"));

            sf::FloatRect bounds = heart.getLocalBounds();
            heart.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

            float heartSize = 24.f;
            heart.setScale(heartSize / bounds.width, heartSize / bounds.height);

            heart.setPosition(
                Constants::WINDOW_W - 95.f + i * 28.f,
                20.f
            );

            window.draw(heart);
        }
    }
    else {
        std::string livesText = "";
        for (int i = 0; i < player.lives; ++i) {
            livesText += "<3 ";
        }

        drawText(
            window,
            livesText,
            Constants::WINDOW_W - 95.f,
            8.f,
            18,
            sf::Color(255, 80, 80)
        );
    }
    drawText(window, "KILLED: " + std::to_string(killedEnemies) + "/" + std::to_string(totalEnemies),
             Constants::WINDOW_W / 2.f - 75.f, 8.f, 18, sf::Color(220, 220, 220));
    if (escapedEnemies > 0) {
        drawText(window, "ESCAPED: " + std::to_string(escapedEnemies),
                 Constants::WINDOW_W / 2.f - 65.f, 32.f, 16, sf::Color(255, 120, 80));
    }
    if (player.eggs > 0) {
        drawText(window, "BOMBS: " + std::to_string(player.eggs), 10.f, 56.f, 16, sf::Color(255, 60, 200));
    }
}

void HUD::drawCentered(sf::RenderWindow& window, const std::string& title,
                       const std::string& subtitle, const std::string& subtitle2) {
    if (!fontLoaded) return;
    auto centerText = [&](const std::string& text, float y, unsigned int size, sf::Color color) {
        sf::Text drawableText;
        drawableText.setFont(font);
        drawableText.setString(text);
        drawableText.setCharacterSize(size);
        drawableText.setFillColor(color);
        const sf::FloatRect bounds = drawableText.getLocalBounds();
        drawableText.setPosition((Constants::WINDOW_W - bounds.width) / 2.f, y);
        window.draw(drawableText);
    };
    centerText(title, Constants::WINDOW_H / 2.f - 90.f, 44, sf::Color(255, 220, 50));
    centerText(subtitle, Constants::WINDOW_H / 2.f - 10.f, 23, sf::Color(230, 230, 230));
    if (!subtitle2.empty()) centerText(subtitle2, Constants::WINDOW_H / 2.f + 35.f, 19, sf::Color(185, 185, 185));
}

bool HUD::isLoaded() const { return fontLoaded; }
