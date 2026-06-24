#include "AssetManager.hpp"
#include <iostream>

std::map<std::string, sf::Texture> AssetManager::textures;

bool AssetManager::load(const std::string& key, const std::string& path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return false;
    }
    texture.setSmooth(true);
    textures[key] = texture;
    return true;
}

void AssetManager::loadAll() {
    load("player", "assets/player_ship.png");
    load("enemy_normal", "assets/enemy_chicken.png");
    load("enemy_fast", "assets/enemy_fast.png");
    load("enemy_tank", "assets/enemy_tank.png");
    load("enemy_spinner", "assets/enemy_spinner.png");
    load("boss", "assets/boss_chicken.png");
    load("bullet_player", "assets/bullet_player.png");
    load("bullet_enemy", "assets/bullet_enemy.png");
    load("power_multi", "assets/power_multishot.png");
    load("power_life", "assets/power_life.png");
    load("power_bomb", "assets/power_bomb.png");
    load("star", "assets/star.png");
    load("heart", "assets/heart.png");
}

bool AssetManager::has(const std::string& key) {
    return textures.find(key) != textures.end();
}

const sf::Texture& AssetManager::get(const std::string& key) {
    return textures.at(key);
}
