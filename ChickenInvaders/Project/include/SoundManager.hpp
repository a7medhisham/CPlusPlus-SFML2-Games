#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <vector>

class SoundManager {
public:
    static void loadAll();

    static void play(const std::string& key);
    static void update();

    static void setVolume(float volume);

private:
    static bool load(const std::string& key, const std::string& path);

    static std::map<std::string, sf::SoundBuffer> buffers;
    static std::vector<sf::Sound> activeSounds;

    static float globalVolume;
};
