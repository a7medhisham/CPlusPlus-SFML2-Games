#pragma once
#include "Bullet.hpp"
#include "HUD.hpp"
#include "LevelConfig.hpp"
#include "Particle.hpp"
#include "Player.hpp"
#include "PowerUp.hpp"
#include "Starfield.hpp"
#include "enemies/Enemy.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Game {
public:
    Game();
    void run();
private:
    enum class State { Menu, Playing, LevelComplete, GameOver, Victory };
    void processEvents();
    void handleKeyPressed(sf::Keyboard::Key key);
    void update(float dt);
    void updatePlaying(float dt);
    void updatePlayerInput(float dt);
    void render();
    void loadLevel(int level);
    void startNewGame();
    void goToNextLevel();
    void spawnExplosion(float x, float y, sf::Color color, int count = 18);
    void appendBullets(std::vector<std::unique_ptr<Bullet>> newBullets);
    void handleEnemyEscapes();
    void handleCollisions();
    void handleBombInput();
    void activateBomb();
    void cleanupDeadObjects();
    void checkGameProgress();

    sf::RenderWindow window;
    sf::Clock clock;
    Player player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<Particle> particles;
    std::vector<PowerUp> powerUps;
    Starfield stars;
    HUD hud;
    int currentLevel = 1;
    int totalEnemies = 0;
    int killedEnemies = 0;
    int escapedEnemies = 0;
    bool bombKeyWasDown = false;
    bool bombFlashActive = false;
    float bombFlashTimer = 0.f;
    float escapedWarningTimer = 0.f;
    State state = State::Menu;
};
