#include "Game.hpp"

#include "AssetManager.hpp"
#include "SoundManager.hpp"
#include "Constants.hpp"
#include "enemies/BossEnemy.hpp"
#include "enemies/FastEnemy.hpp"
#include "enemies/NormalEnemy.hpp"
#include "enemies/SpinnerEnemy.hpp"
#include "enemies/TankEnemy.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>

Game::Game()
    : window(
        sf::VideoMode(Constants::WINDOW_W, Constants::WINDOW_H),
        "Chicken Invaders - OOP Version",
        sf::Style::Close
    ) {
    window.setFramerateLimit(60);

    srand(static_cast<unsigned>(time(nullptr)));

    AssetManager::loadAll();
    SoundManager::loadAll();
    SoundManager::setVolume(70.f);

    if (AssetManager::has("player")) {
        player.setTexture(AssetManager::get("player"));
    }

    loadLevel(1);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 1.f / 30.f);

        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event{};

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            handleKeyPressed(event.key.code);
        }
    }
}

void Game::handleKeyPressed(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        window.close();
        return;
    }

    if (key != sf::Keyboard::Enter) {
        return;
    }

    if (state == State::Menu || state == State::GameOver || state == State::Victory) {
        startNewGame();
    }
    else if (state == State::LevelComplete) {
        goToNextLevel();
    }
}

void Game::update(float dt) {
    SoundManager::update();

    stars.update(dt);

    if (escapedWarningTimer > 0.f) {
        escapedWarningTimer = std::max(0.f, escapedWarningTimer - dt);
    }

    if (bombFlashActive) {
        bombFlashTimer -= dt;
        if (bombFlashTimer <= 0.f) {
            bombFlashActive = false;
        }
    }

    if (state == State::Playing) {
        updatePlaying(dt);
    }
    else {
        for (auto& particle : particles) {
            particle.update(dt);
        }

        particles.erase(
            std::remove_if(
                particles.begin(),
                particles.end(),
                [](const Particle& particle) { return !particle.alive; }
            ),
            particles.end()
        );
    }
}

void Game::updatePlaying(float dt) {
    updatePlayerInput(dt);
    handleBombInput();

    player.update(dt);

    for (auto& enemy : enemies) {
        if (!enemy->alive) {
            continue;
        }

        const bool shouldShoot = enemy->updateEnemy(dt);
        if (shouldShoot) {
            appendBullets(enemy->shoot());
        }
    }

    handleEnemyEscapes();

    for (auto& bullet : bullets) {
        bullet->update(dt);
    }

    for (auto& powerUp : powerUps) {
        powerUp.update(dt);
    }

    for (auto& particle : particles) {
        particle.update(dt);
    }

    handleCollisions();
    cleanupDeadObjects();
    checkGameProgress();
}

void Game::updatePlayerInput(float dt) {
    float dx = 0.f;
    float dy = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        dx -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        dx += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        dy -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        dy += 1.f;
    }

    if (dx != 0.f && dy != 0.f) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }

    player.x += dx * Constants::PLAYER_SPEED * dt;
    player.y += dy * Constants::PLAYER_SPEED * dt;

    const bool shootPressed =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::J) ||
        sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (shootPressed) {
        auto newBullets = player.shoot();

        if (!newBullets.empty()) {
            SoundManager::play("shoot");
        }

        appendBullets(std::move(newBullets));
    }
}

void Game::render() {
    window.clear(sf::Color(9, 11, 25));

    stars.draw(window);

    for (auto& powerUp : powerUps) {
        powerUp.draw(window);
    }

    for (auto& bullet : bullets) {
        bullet->draw(window);
    }

    for (auto& enemy : enemies) {
        enemy->draw(window);
    }

    if (state == State::Playing || state == State::LevelComplete || state == State::GameOver || state == State::Victory) {
        player.draw(window);
    }

    for (auto& particle : particles) {
        particle.draw(window);
    }

    if (bombFlashActive) {
        sf::RectangleShape flash({ static_cast<float>(Constants::WINDOW_W), static_cast<float>(Constants::WINDOW_H) });
        flash.setFillColor(sf::Color(255, 255, 255, 60));
        window.draw(flash);
    }

    hud.drawGameplay(window, player, currentLevel, totalEnemies, killedEnemies, escapedEnemies);

    if (escapedWarningTimer > 0.f) {
        hud.drawCentered(window, "ENEMY ESCAPED!", "-1 LIFE", "");
    }

    if (state == State::Menu) {
        hud.drawCentered(
            window,
            "CHICKEN INVADERS",
            "Press ENTER to Start",
            "Move: WASD/Arrows | Shoot: Space/Mouse | Bomb: B or E"
        );
    }
    else if (state == State::LevelComplete) {
        hud.drawCentered(
            window,
            "LEVEL COMPLETE",
            "Press ENTER for Level " + std::to_string(currentLevel + 1),
            "Score: " + std::to_string(player.score)
        );
    }
    else if (state == State::GameOver) {
        hud.drawCentered(
            window,
            "GAME OVER",
            "Press ENTER to Restart",
            "Final Score: " + std::to_string(player.score)
        );
    }
    else if (state == State::Victory) {
        hud.drawCentered(
            window,
            "VICTORY!",
            "You finished all 10 levels",
            "Final Score: " + std::to_string(player.score) + " | Press ENTER to play again"
        );
    }

    window.display();
}

void Game::loadLevel(int level) {
    currentLevel = level;

    enemies.clear();
    bullets.clear();
    particles.clear();
    powerUps.clear();

    killedEnemies = 0;
    escapedEnemies = 0;
    escapedWarningTimer = 0.f;
    bombFlashActive = false;
    bombFlashTimer = 0.f;
    bombKeyWasDown = false;

    player.setLevel(level);

    const LevelConfig config = getLevelConfig(level);

    totalEnemies =
        config.normalCount +
        config.fastCount +
        config.tankCount +
        config.spinnerCount +
        (config.hasBoss ? 1 : 0);

    int spawnIndex = 0;

    auto spawnPosition = [&](int index) {
        const int columns = 8;
        const int col = index % columns;
        const int row = index / columns;

        const float x = 80.f + static_cast<float>(col) * 90.f;
        const float y = -70.f - static_cast<float>(row) * 70.f;

        return sf::Vector2f(x, y);
        };

    for (int i = 0; i < config.normalCount; ++i) {
        const sf::Vector2f pos = spawnPosition(spawnIndex++);
        enemies.push_back(std::make_unique<NormalEnemy>(pos.x, pos.y, level, config.speedMultiplier, config.shootMultiplier));
    }

    for (int i = 0; i < config.fastCount; ++i) {
        const sf::Vector2f pos = spawnPosition(spawnIndex++);
        enemies.push_back(std::make_unique<FastEnemy>(pos.x, pos.y, level, config.speedMultiplier, config.shootMultiplier));
    }

    for (int i = 0; i < config.tankCount; ++i) {
        const sf::Vector2f pos = spawnPosition(spawnIndex++);
        enemies.push_back(std::make_unique<TankEnemy>(pos.x, pos.y, level, config.speedMultiplier, config.shootMultiplier));
    }

    for (int i = 0; i < config.spinnerCount; ++i) {
        const sf::Vector2f pos = spawnPosition(spawnIndex++);
        enemies.push_back(std::make_unique<SpinnerEnemy>(pos.x, pos.y, level, config.speedMultiplier, config.shootMultiplier));
    }

    if (config.hasBoss) {
        enemies.push_back(std::make_unique<BossEnemy>(
            Constants::WINDOW_W / 2.f,
            -110.f,
            level
        ));
    }
}

void Game::startNewGame() {
    player = Player();

    if (AssetManager::has("player")) {
        player.setTexture(AssetManager::get("player"));
    }

    loadLevel(1);
    state = State::Playing;
}

void Game::goToNextLevel() {
    if (currentLevel >= 10) {
        state = State::Victory;
        return;
    }

    loadLevel(currentLevel + 1);
    state = State::Playing;
}

void Game::spawnExplosion(float x, float y, sf::Color color, int count) {
    for (int i = 0; i < count; ++i) {
        particles.emplace_back(x, y, color);
    }
}

void Game::appendBullets(std::vector<std::unique_ptr<Bullet>> newBullets) {
    for (auto& bullet : newBullets) {
        bullets.push_back(std::move(bullet));
    }
}

void Game::handleEnemyEscapes() {
    for (auto& enemy : enemies) {
        if (!enemy->escaped) {
            continue;
        }

        enemy->escaped = false;
        escapedEnemies++;
        escapedWarningTimer = 0.9f;

        player.loseLifeFromEscape();
        spawnExplosion(enemy->x, Constants::WINDOW_H - 20.f, sf::Color(255, 90, 40), 12);
    }
}

void Game::handleCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet->alive || bullet->isEnemy) {
            continue;
        }

        for (auto& enemy : enemies) {
            if (!enemy->alive) {
                continue;
            }

            if (!bullet->intersects(*enemy)) {
                continue;
            }

            bullet->alive = false;

            const bool enemyDied = enemy->takeDamage(1);
            spawnExplosion(bullet->x, bullet->y, sf::Color(120, 230, 255), 5);

            if (enemyDied) {
                killedEnemies++;
                player.score += enemy->getScoreValue();

                SoundManager::play("chicken");
                SoundManager::play("explosion");

                spawnExplosion(enemy->x, enemy->y, sf::Color(255, 210, 80), enemy->isBoss() ? 70 : 22);

                const int dropChance = enemy->isBoss() ? 100 : 15;
                if ((rand() % 100) < dropChance) {
                    powerUps.emplace_back(enemy->x, enemy->y);
                }
            }

            break;
        }
    }

    for (auto& bullet : bullets) {
        if (!bullet->alive || !bullet->isEnemy) {
            continue;
        }

        if (!bullet->intersects(player)) {
            continue;
        }

        bullet->alive = false;

        if (player.hit()) {
            spawnExplosion(player.x, player.y, sf::Color(255, 80, 80), 30);
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy->alive || enemy->isBoss()) {
            continue;
        }

        if (!enemy->intersects(player)) {
            continue;
        }

        enemy->alive = false;
        killedEnemies++;
        player.score += enemy->getScoreValue() / 2;

        SoundManager::play("chicken");
        SoundManager::play("explosion");

        spawnExplosion(enemy->x, enemy->y, sf::Color(255, 160, 60), 20);

        if (player.hit()) {
            spawnExplosion(player.x, player.y, sf::Color(255, 80, 80), 30);
        }
    }

    for (auto& powerUp : powerUps) {
        if (!powerUp.alive) {
            continue;
        }

        if (!powerUp.getBounds().intersects(player.getBounds())) {
            continue;
        }

        powerUp.alive = false;

        SoundManager::play("powerup");

        switch (powerUp.getType()) {
        case PowerUp::Type::MultiShot:
            player.multiShot = true;
            player.score += 100;
            break;

        case PowerUp::Type::Life:
            player.lives++;
            player.score += 150;
            break;

        case PowerUp::Type::Bomb:
            player.eggs++;
            player.score += 120;
            break;
        }

        spawnExplosion(powerUp.x, powerUp.y, sf::Color(100, 255, 140), 18);
    }
}

void Game::handleBombInput() {
    const bool bombKeyDown =
        sf::Keyboard::isKeyPressed(sf::Keyboard::B) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::E);

    if (bombKeyDown && !bombKeyWasDown) {
        activateBomb();
    }

    bombKeyWasDown = bombKeyDown;
}

void Game::activateBomb() {
    if (player.eggs <= 0) {
        return;
    }

    player.eggs--;

    SoundManager::play("bomb");

    bombFlashActive = true;
    bombFlashTimer = 0.18f;

    for (auto& bullet : bullets) {
        if (bullet->isEnemy) {
            bullet->alive = false;
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy->alive) {
            continue;
        }

        if (enemy->isBoss()) {
            const bool bossDied = enemy->takeDamage(10);
            spawnExplosion(enemy->x, enemy->y, sf::Color(255, 80, 80), 40);

            if (bossDied) {
                killedEnemies++;
                player.score += enemy->getScoreValue();

                SoundManager::play("explosion");

                powerUps.emplace_back(enemy->x, enemy->y);
            }

            continue;
        }

        enemy->alive = false;
        killedEnemies++;
        player.score += enemy->getScoreValue();

        SoundManager::play("chicken");
        SoundManager::play("explosion");

        spawnExplosion(enemy->x, enemy->y, sf::Color(255, 200, 90), 22);
    }
}

void Game::cleanupDeadObjects() {
    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](const std::unique_ptr<Bullet>& bullet) { return !bullet->alive; }
        ),
        bullets.end()
    );

    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),
            [](const std::unique_ptr<Enemy>& enemy) { return !enemy->alive; }
        ),
        enemies.end()
    );

    powerUps.erase(
        std::remove_if(
            powerUps.begin(),
            powerUps.end(),
            [](const PowerUp& powerUp) { return !powerUp.alive; }
        ),
        powerUps.end()
    );

    particles.erase(
        std::remove_if(
            particles.begin(),
            particles.end(),
            [](const Particle& particle) { return !particle.alive; }
        ),
        particles.end()
    );
}

void Game::checkGameProgress() {
    if (player.lives <= 0) {
        player.lives = 0;

        SoundManager::play("game_over");

        state = State::GameOver;
        return;
    }

    if (!enemies.empty()) {
        return;
    }

    if (currentLevel >= 10) {
        SoundManager::play("victory");
        state = State::Victory;
    }
    else {
        SoundManager::play("level_complete");
        state = State::LevelComplete;
    }
}
