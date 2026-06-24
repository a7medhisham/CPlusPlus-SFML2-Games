#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class AssetManager {
public:
    static void loadAll();
    static bool has(const std::string& key);
    static const sf::Texture& get(const std::string& key);
private:
    static bool load(const std::string& key, const std::string& path);
    static std::map<std::string, sf::Texture> textures;
};
