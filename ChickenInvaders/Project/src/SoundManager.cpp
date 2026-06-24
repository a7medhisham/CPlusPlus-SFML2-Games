#include "SoundManager.hpp"
#include <algorithm>
#include <iostream>

std::map<std::string, sf::SoundBuffer> SoundManager::buffers;
std::vector<sf::Sound> SoundManager::activeSounds;
float SoundManager::globalVolume = 100.f;

bool SoundManager::load(const std::string& key, const std::string& path) {
    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(path)) {
        std::cout << "FAILED TO LOAD SOUND: " << path << std::endl;
        return false;
    }

    std::cout << "Loaded sound: " << path << std::endl;
    buffers[key] = buffer;
    return true;
}

void SoundManager::loadAll() {
    activeSounds.reserve(50);

    load("shoot", "assets/sounds/player_shoot.wav");
    load("explosion", "assets/sounds/explosion.wav");
    load("chicken", "assets/sounds/chicken.wav");
    load("powerup", "assets/sounds/powerup.wav");
    load("game_over", "assets/sounds/game_over.wav");
    load("victory", "assets/sounds/victory.wav");
    load("level_complete", "assets/sounds/level_complete.wav");
    load("bomb", "assets/sounds/bomb.wav");
}

void SoundManager::play(const std::string& key) {
    auto it = buffers.find(key);

    if (it == buffers.end()) {
        std::cout << "SOUND KEY NOT FOUND: " << key << std::endl;
        return;
    }

    activeSounds.emplace_back();
    sf::Sound& sound = activeSounds.back();

    sound.setBuffer(it->second);
    sound.setVolume(globalVolume);
    sound.play();

    std::cout << "Playing sound: " << key << std::endl;
}

void SoundManager::update() {
    activeSounds.erase(
        std::remove_if(
            activeSounds.begin(),
            activeSounds.end(),
            [](const sf::Sound& sound) {
                return sound.getStatus() == sf::Sound::Stopped;
            }
        ),
        activeSounds.end()
    );
}

void SoundManager::setVolume(float volume) {
    globalVolume = volume;
}