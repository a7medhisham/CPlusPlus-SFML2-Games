#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Constants
const int   WINDOW_W = 800;
const int   WINDOW_H = 600;
const float PLAYER_SPEED = 320.f;
const float BASE_SPEED = 280.f;
const float SPEED_INCREMENT = 18.f;
const int   MAX_MISSED = 5;
const float SPAWN_START = 0.85f;
const float SPAWN_FASTEST = 0.35f;

// Utility
bool loadTexture(sf::Texture& tex, const std::string& path)
{
    if (!tex.loadFromFile(path)) {
        sf::Image img;
        img.create(64, 64, sf::Color(200, 50, 200));
        tex.loadFromImage(img);
        return false;
    }
    tex.setSmooth(true);
    return true;
}

// Audio Manager
struct AudioManager
{
    sf::SoundBuffer bufCollect;
    sf::SoundBuffer bufMiss;
    sf::SoundBuffer bufGameOver;
    sf::Music       bgMusic;
    sf::Sound       sndCollect;
    sf::Sound       sndMiss;
    sf::Sound       sndGameOver;
    bool ready = false;

    void load()
    {
        bool ok = bufCollect.loadFromFile("assets/collect.wav")
            && bufMiss.loadFromFile("assets/miss.wav")
            && bufGameOver.loadFromFile("assets/gameover.wav");
        if (ok) {
            sndCollect.setBuffer(bufCollect);
            sndMiss.setBuffer(bufMiss);
            sndGameOver.setBuffer(bufGameOver);
            ready = true;
        }
        if (bgMusic.openFromFile("assets/bgmusic.wav")) {
            bgMusic.setLoop(true);
            bgMusic.setVolume(45.f);
            bgMusic.play();
        }
    }

    void playCollect() {
        if (!ready) return;
        sndCollect.setPitch(0.9f + (std::rand() % 40) / 100.f);
        sndCollect.setVolume(85.f);
        sndCollect.play();
    }

    void playMiss() {
        if (!ready) return;
        sndMiss.setVolume(70.f);
        sndMiss.play();
    }

    void playGameOver() {
        if (!ready) return;
        bgMusic.stop();
        sndGameOver.setVolume(90.f);
        sndGameOver.play();
    }

    void restartMusic() {
        bgMusic.stop();
        if (bgMusic.openFromFile("assets/bgmusic.wav")) {
            bgMusic.setLoop(true);
            bgMusic.setVolume(45.f);
            bgMusic.play();
        }
    }
};

// Particle
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f    velocity;
    float life = 1.f;
    float maxLife = 0.5f;
};

// Food
struct FoodTextures { std::array<sf::Texture, 6> tex; };

class Food {
public:
    sf::Sprite sprite;
    bool       collected = false;
    int        typeIndex = 0;

    Food(float x, FoodTextures& ft, float speed) : speed_(speed) {
        typeIndex = std::rand() % 6;
        sprite.setTexture(ft.tex[typeIndex]);
        sf::Vector2u ts = ft.tex[typeIndex].getSize();
        float scale = 44.f / static_cast<float>(std::max(ts.x, ts.y));
        sprite.setScale(scale, scale);
        sprite.setOrigin(ts.x / 2.f, ts.y / 2.f);
        sprite.setPosition(x, -30.f);
        rotSpeed_ = (std::rand() % 2 == 0 ? 1.f : -1.f) * (30.f + std::rand() % 60);
        wobblePhase_ = static_cast<float>(std::rand() % 628) / 100.f;
        wobbleAmp_ = 3.f + std::rand() % 5;
    }

    void update(float dt) {
        elapsed_ += dt;
        float wobble = std::sin(wobblePhase_ + elapsed_ * 3.f) * wobbleAmp_;
        sprite.move(wobble * dt, speed_ * dt);
        sprite.rotate(rotSpeed_ * dt);
    }

    bool isOffScreen() const { return sprite.getPosition().y - 30.f > WINDOW_H; }
    sf::FloatRect bounds() const { return sprite.getGlobalBounds(); }

private:
    float speed_ = BASE_SPEED, rotSpeed_ = 0,
        wobblePhase_ = 0, wobbleAmp_ = 0, elapsed_ = 0;
};

// Player
class Player {
public:
    sf::Sprite  sprite;
    sf::Texture texture;
    int         score = 0;
    float       thrustTimer = 0.f;

    void init() {
        loadTexture(texture, "assets/player.png");
        sprite.setTexture(texture);
        sf::Vector2u ts = texture.getSize();
        sprite.setOrigin(ts.x / 2.f, ts.y / 2.f);
        float scale = 56.f / static_cast<float>(std::max(ts.x, ts.y));
        sprite.setScale(scale, scale);
        sprite.setPosition(WINDOW_W / 2.f, WINDOW_H - 50.f);
    }

    void handleInput(float dt) {
        float dx = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A))    dx = -PLAYER_SPEED * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D))    dx = PLAYER_SPEED * dt;
        sprite.move(dx, 0.f);
        float px = sprite.getPosition().x;
        float half = sprite.getGlobalBounds().width / 2.f;
        if (px - half < 0)        sprite.setPosition(half, sprite.getPosition().y);
        if (px + half > WINDOW_W) sprite.setPosition(WINDOW_W - half, sprite.getPosition().y);
        float target = (dx < 0) ? -14.f : (dx > 0) ? 14.f : 0.f;
        float cur = sprite.getRotation(); if (cur > 180) cur -= 360;
        sprite.setRotation(cur + (target - cur) * dt * 10.f);
        thrustTimer += dt;
    }

    sf::FloatRect bounds() const { return sprite.getGlobalBounds(); }
};

// Helper
static void setupText(sf::Text& t, sf::Font& f, unsigned sz,
    sf::Color c, float x, float y) {
    t.setFont(f); t.setCharacterSize(sz);
    t.setFillColor(c); t.setPosition(x, y);
}

// Main
int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_W, WINDOW_H),
        "Food Collector  -  SFML",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(120);

    AudioManager audio;
    audio.load();

    // Textures
    sf::Texture bgTex, heartTex;
    loadTexture(bgTex, "assets/background.png");
    sf::Sprite bgSprite(bgTex);
    bgSprite.setScale(
        static_cast<float>(WINDOW_W) / bgTex.getSize().x,
        static_cast<float>(WINDOW_H) / bgTex.getSize().y
    );
    loadTexture(heartTex, "assets/heart.png");

    FoodTextures ft;
    loadTexture(ft.tex[0], "assets/apple.png");
    loadTexture(ft.tex[1], "assets/banana.png");
    loadTexture(ft.tex[2], "assets/cherry.png");
    loadTexture(ft.tex[3], "assets/grape.png");
    loadTexture(ft.tex[4], "assets/orange.png");
    loadTexture(ft.tex[5], "assets/star.png");

    // Font
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf"))
        font.loadFromFile("arial.ttf");

    // Texts
    sf::Text scoreText, speedText, controlsText, gameOverText, restartText;
    setupText(scoreText, font, 28, sf::Color::White, 14.f, 10.f);
    setupText(speedText, font, 22, sf::Color(160, 220, 255), 14.f, 48.f);
    setupText(controlsText, font, 18, sf::Color(180, 180, 180, 180),
        WINDOW_W / 2.f - 130.f, WINDOW_H - 26.f);
    controlsText.setString("Arrow keys / A D  to move");

    setupText(gameOverText, font, 52, sf::Color(255, 80, 80), 0.f, 0.f);
    gameOverText.setString("GAME  OVER");
    {
        auto b = gameOverText.getLocalBounds();
        gameOverText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        gameOverText.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f - 55.f);
    }

    setupText(restartText, font, 26, sf::Color(255, 220, 80), 0.f, 0.f);
    restartText.setString("Press  R  to play again");
    {
        auto b = restartText.getLocalBounds();
        restartText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        restartText.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f + 20.f);
    }

    // UI shapes
    sf::RectangleShape hudPanel(sf::Vector2f(200.f, 80.f));
    hudPanel.setFillColor(sf::Color(0, 0, 0, 120));
    hudPanel.setOutlineColor(sf::Color(255, 255, 255, 30));
    hudPanel.setOutlineThickness(1.f);
    hudPanel.setPosition(8.f, 4.f);

    sf::RectangleShape dimOverlay(sf::Vector2f(WINDOW_W, WINDOW_H));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    sf::RectangleShape goPanel(sf::Vector2f(380.f, 170.f));
    goPanel.setFillColor(sf::Color(20, 10, 40, 220));
    goPanel.setOutlineColor(sf::Color(255, 80, 80, 180));
    goPanel.setOutlineThickness(2.f);
    goPanel.setOrigin(190.f, 85.f);
    goPanel.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f);

    // Player
    Player player;
    player.init();

    // State
    std::vector<Food>     foods;
    std::vector<Particle> particles;
    float obstacleSpeed = BASE_SPEED;
    float spawnTimer = 0.f;
    float spawnInterval = SPAWN_START;
    int   missedObjects = 0;
    bool  gameOver = false;
    bool  goSoundPlayed = false;
    float goFlashTimer = 0.f;

    auto resetGame = [&]() {
        gameOver = false;
        goSoundPlayed = false;
        goFlashTimer = 0.f;
        player.score = 0;
        missedObjects = 0;
        obstacleSpeed = BASE_SPEED;
        spawnInterval = SPAWN_START;
        spawnTimer = 0.f;
        foods.clear();
        particles.clear();
        player.sprite.setPosition(WINDOW_W / 2.f, WINDOW_H - 50.f);
        player.sprite.setRotation(0.f);
        audio.restartMusic();
        };

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::R && gameOver) resetGame();
                if (ev.key.code == sf::Keyboard::Escape)        window.close();
            }
        }

        if (!gameOver)
        {
            player.handleInput(dt);

            spawnTimer += dt;
            if (spawnTimer >= spawnInterval) {
                spawnTimer = 0.f;
                float rx = 30.f + static_cast<float>(std::rand() % (WINDOW_W - 60));
                foods.emplace_back(rx, ft, obstacleSpeed);
                spawnInterval = std::max(SPAWN_FASTEST, spawnInterval - 0.003f);
            }

            for (int i = static_cast<int>(foods.size()) - 1; i >= 0; --i) {
                foods[i].update(dt);

                if (!foods[i].collected &&
                    foods[i].bounds().intersects(player.bounds()))
                {
                    foods[i].collected = true;
                    player.score++;
                    obstacleSpeed += SPEED_INCREMENT;
                    audio.playCollect();

                    sf::Vector2f fp = foods[i].sprite.getPosition();
                    for (int p = 0; p < 12; ++p) {
                        float angle = (std::rand() % 360) * 3.14159f / 180.f;
                        float spd = 60.f + std::rand() % 120;
                        Particle par;
                        par.shape.setRadius(3.f + std::rand() % 3);
                        par.shape.setFillColor(sf::Color(
                            200 + std::rand() % 55, 150 + std::rand() % 105,
                            50 + std::rand() % 100, 255));
                        par.shape.setPosition(fp);
                        par.velocity = { std::cos(angle) * spd, std::sin(angle) * spd };
                        par.life = 1.f;
                        par.maxLife = 0.5f + (std::rand() % 3) * 0.1f;
                        particles.push_back(par);
                    }
                }

                if (foods[i].isOffScreen() && !foods[i].collected) {
                    missedObjects++;
                    audio.playMiss();
                    foods.erase(foods.begin() + i);
                }
                else if (foods[i].collected) {
                    foods.erase(foods.begin() + i);
                }
            }

            for (int i = static_cast<int>(particles.size()) - 1; i >= 0; --i) {
                auto& p = particles[i];
                p.shape.move(p.velocity * dt);
                p.velocity.y += 200.f * dt;
                p.life -= dt / p.maxLife;
                if (p.life <= 0.f) { particles.erase(particles.begin() + i); continue; }
                sf::Color c = p.shape.getFillColor();
                c.a = static_cast<sf::Uint8>(255 * std::max(0.f, p.life));
                p.shape.setFillColor(c);
            }

            if (missedObjects >= MAX_MISSED) gameOver = true;

            {
                std::ostringstream ss;
                ss << "Score:  " << player.score;
                scoreText.setString(ss.str());
                ss.str("");
                ss << "Speed:  "
                    << static_cast<int>((obstacleSpeed / BASE_SPEED) * 100.f) << "%";
                speedText.setString(ss.str());
            }
        }
        else
        {
            goFlashTimer += dt;
            if (!goSoundPlayed) {
                audio.playGameOver();
                goSoundPlayed = true;
            }
        }

        // Render
        window.clear(sf::Color(10, 8, 28));
        window.draw(bgSprite);

        for (auto& f : foods)
            if (!f.collected) window.draw(f.sprite);

        for (auto& p : particles) window.draw(p.shape);

        {
            float pulse = 0.6f + 0.4f * std::sin(player.thrustTimer * 15.f);
            sf::CircleShape glow(14.f * pulse);
            glow.setFillColor(sf::Color(255, 160, 0,
                static_cast<sf::Uint8>(120 * pulse)));
            glow.setOrigin(14.f * pulse, 14.f * pulse);
            auto pp = player.sprite.getPosition();
            glow.setPosition(pp.x, pp.y + 20.f);
            window.draw(glow);
        }

        window.draw(player.sprite);
        window.draw(hudPanel);
        window.draw(scoreText);
        window.draw(speedText);

        int remaining = MAX_MISSED - missedObjects;
        for (int i = 0; i < MAX_MISSED; ++i) {
            sf::Sprite heart(heartTex);
            float sc = 22.f / static_cast<float>(
                std::max(heartTex.getSize().x, heartTex.getSize().y));
            heart.setScale(sc, sc);
            heart.setPosition(WINDOW_W - 30.f - i * 28.f, 12.f);
            if (i >= remaining) heart.setColor(sf::Color(255, 255, 255, 50));
            window.draw(heart);
        }

        window.draw(controlsText);

        if (gameOver) {
            window.draw(dimOverlay);
            window.draw(goPanel);
            float alpha = 180.f + 75.f * std::sin(goFlashTimer * 4.f);
            gameOverText.setFillColor(sf::Color(255, 80, 80,
                static_cast<sf::Uint8>(alpha)));
            window.draw(gameOverText);

            std::ostringstream oss;
            oss << "Final Score:  " << player.score;
            sf::Text fs;
            setupText(fs, font, 24, sf::Color(255, 220, 100), 0.f, 0.f);
            fs.setString(oss.str());
            {
                auto b = fs.getLocalBounds();
                fs.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
                fs.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f - 5.f);
            }
            window.draw(fs);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}
