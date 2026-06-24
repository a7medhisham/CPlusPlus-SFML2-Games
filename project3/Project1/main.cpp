#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

// Constants
const int   WIN_W = 800;
const int   WIN_H = 600;

const float GRAVITY = 980.f;   // px/s²
const float FLAP_VELOCITY = -320.f;   // px/s  upward
const float INITIAL_PIPE_SPD = 180.f;   // px/s
const float SPEED_INCREMENT = 30.f;
const int   LEVEL_EVERY = 10;     // pipes

const float PIPE_WIDTH = 62.f;
const float PIPE_GAP = 185.f;
const float PIPE_SPACING = 270.f;
const int   PIPES_ON_SCR = 4;

const float GROUND_Y = 518.f;

// Utility – safe loader
bool loadTex(sf::Texture& t, const std::string& p)
{
    if (t.loadFromFile(p)) { t.setSmooth(true); return true; }
    if (t.loadFromFile("assets/" + p)) { t.setSmooth(true); return true; }
    sf::Image img; img.create(64, 64, sf::Color(200, 50, 200));
    t.loadFromImage(img); return false;
}

bool loadBuf(sf::SoundBuffer& b, const std::string& p)
{
    if (b.loadFromFile(p))           return true;
    if (b.loadFromFile("assets/" + p)) return true;
    return false;
}

// Audio Manager
struct Audio
{
    sf::SoundBuffer bFlap, bScore, bHit, bDie, bLevel;
    sf::Sound       sFlap, sScore, sHit, sDie, sLevel;

    void load()
    {
        loadBuf(bFlap, "flap.wav");
        loadBuf(bScore, "score.wav");
        loadBuf(bHit, "hit.wav");
        loadBuf(bDie, "die.wav");
        loadBuf(bLevel, "levelup.wav");
        sFlap.setBuffer(bFlap);
        sScore.setBuffer(bScore);
        sHit.setBuffer(bHit);
        sDie.setBuffer(bDie);
        sLevel.setBuffer(bLevel);
        sFlap.setVolume(80);
        sScore.setVolume(90);
        sHit.setVolume(95);
        sDie.setVolume(95);
        sLevel.setVolume(90);
    }
    void flap() { sFlap.play(); }
    void score() { sScore.play(); }
    void hit() { sHit.play(); }
    void die() { sDie.play(); }
    void levelup() { sLevel.play(); }
};
// Particle
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f    vel;
    float life = 1.f;
};

// Pipe
struct PipeTextures { sf::Texture body, cap; };

class Pipe {
public:
    float x;
    float gapTop;   // y where gap starts
    bool  counted = false;

    Pipe(float x_, float gapTop_, PipeTextures& pt)
        : x(x_), gapTop(gapTop_), pt_(&pt) {}

    void update(float spd, float dt) { x -= spd * dt; }

    bool isOffScreen() const { return x + PIPE_WIDTH < 0; }

    bool collides(const sf::FloatRect& b) const
    {
        // top pipe rect
        sf::FloatRect top(x, 0, PIPE_WIDTH, gapTop);
        // bottom pipe rect
        sf::FloatRect bot(x, gapTop + PIPE_GAP,
            PIPE_WIDTH, WIN_H - gapTop - PIPE_GAP);
        return top.intersects(b) || bot.intersects(b);
    }

    void draw(sf::RenderWindow& w) const
    {
        float capH = (float)pt_->cap.getSize().y;
        float bodyW = (float)pt_->body.getSize().x;
        float capW = (float)pt_->cap.getSize().x;

        // TOP pipe 
        // body (tiled downward from y=0 to gapTop-capH)
        if (gapTop - capH > 0) {
            sf::Sprite bodyS(pt_->body);
            bodyS.setTextureRect(sf::IntRect(0, 0,
                (int)bodyW, (int)(gapTop - capH)));
            bodyS.setPosition(x + (PIPE_WIDTH - bodyW) / 2.f, 0);
            w.draw(bodyS);
        }
        // cap (at bottom of top pipe)
        {
            sf::Sprite capS(pt_->cap);
            float capX = x + (PIPE_WIDTH - capW) / 2.f;
            capS.setPosition(capX, gapTop - capH);
            w.draw(capS);
        }

        float botY = gapTop + PIPE_GAP;

        // BOTTOM pipe 
        // cap (flipped, at top of bottom pipe)
        {
            sf::Sprite capS(pt_->cap);
            capS.setScale(1.f, -1.f);
            float capX = x + (PIPE_WIDTH - capW) / 2.f;
            capS.setPosition(capX, botY + capH);
            w.draw(capS);
        }
        // body
        float bodyH = WIN_H - botY - capH;
        if (bodyH > 0) {
            sf::Sprite bodyS(pt_->body);
            bodyS.setTextureRect(sf::IntRect(0, 0, (int)bodyW, (int)bodyH));
            bodyS.setPosition(x + (PIPE_WIDTH - bodyW) / 2.f, botY + capH);
            w.draw(bodyS);
        }
    }

private:
    PipeTextures* pt_;
};

// Bird
class Bird {
public:
    sf::Vector2f pos;
    float        vel = 0.f;
    float        angle = 0.f;
    bool         dead = false;

    sf::Texture  texAlive, texDead;

    void init()
    {
        loadTex(texAlive, "bird.png");
        loadTex(texDead, "bird_dead.png");
        reset();
    }

    void reset()
    {
        pos = { WIN_W / 4.f, WIN_H / 2.f };
        vel = 0.f;
        angle = 0.f;
        dead = false;
    }

    void flap() { vel = FLAP_VELOCITY; }

    void update(float dt)
    {
        vel += GRAVITY * dt;
        pos.y += vel * dt;
        // tilt: nose up on flap, nose down on fall
        float targetAngle = std::min(90.f, vel * 0.09f);
        angle += (targetAngle - angle) * dt * 8.f;
    }

    sf::FloatRect bounds() const
    {
        // shrink hitbox slightly for fairness
        float r = 22.f;
        return { pos.x - r, pos.y - r, r * 2.f, r * 2.f };
    }

    bool outOfBounds() const
    {
        return pos.y - 30.f < 0 || pos.y + 30.f >= GROUND_Y;
    }

    void draw(sf::RenderWindow& w) const
    {
        sf::Texture& tex = dead
            ? const_cast<sf::Texture&>(texDead)
            : const_cast<sf::Texture&>(texAlive);
        sf::Sprite s(tex);
        sf::Vector2u ts = tex.getSize();
        s.setOrigin(ts.x / 2.f, ts.y / 2.f);
        float scale = 60.f / std::max(ts.x, ts.y);
        s.setScale(scale, scale);
        s.setPosition(pos);
        s.setRotation(angle);
        w.draw(s);
    }
};

// fill pipes
static float lastPipeX(const std::vector<Pipe>& pipes)
{
    float mx = -1.f;
    for (auto& p : pipes) if (p.x > mx) mx = p.x;
    return mx;
}

static void fillPipes(std::vector<Pipe>& pipes, PipeTextures& pt)
{
    while ((int)pipes.size() < PIPES_ON_SCR) {
        float lx = lastPipeX(pipes);
        float nx = (lx < 0) ? WIN_W : lx + PIPE_WIDTH + PIPE_SPACING;
        float minGap = 80.f, maxGap = GROUND_Y - PIPE_GAP - 80.f;
        float gapTop = minGap + static_cast<float>(std::rand() %
            (int)(maxGap - minGap));
        pipes.emplace_back(nx, gapTop, pt);
    }
}

// setup text helper
static void mkText(sf::Text& t, sf::Font& f,
    unsigned sz, sf::Color c,
    float x, float y,
    const std::string& str = "")
{
    t.setFont(f); t.setCharacterSize(sz);
    t.setFillColor(c); t.setPosition(x, y);
    if (!str.empty()) t.setString(str);
}

// Main
int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode(WIN_W, WIN_H), "Flappy Bird  •  SFML",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(120);

    // Textures
    sf::Texture bgTex;
    loadTex(bgTex, "background.png");
    sf::Sprite bgSprite(bgTex);
    bgSprite.setScale(
        (float)WIN_W / bgTex.getSize().x,
        (float)WIN_H / bgTex.getSize().y);

    PipeTextures pt;
    loadTex(pt.body, "pipe_body.png");
    loadTex(pt.cap, "pipe_cap.png");
    pt.body.setRepeated(true);

    // Audio
    Audio audio;
    audio.load();

    // Font
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf"))
        font.loadFromFile("arial.ttf");

    // Bird
    Bird bird;
    bird.init();

    // State
    std::vector<Pipe>     pipes;
    std::vector<Particle> particles;
    fillPipes(pipes, pt);

    int   score = 0;
    int   pipesPassed = 0;
    float pipeSpeed = INITIAL_PIPE_SPD;
    bool  gameOver = false;
    bool  hitPlayed = false;
    float flashAlpha = 0.f;  // white flash on hit
    float deathTimer = 0.f;

    // Scrolling ground
    float groundScroll = 0.f;

    // HUD texts
    sf::Text scoreTxt, levelTxt, goTxt, restartTxt, bestTxt;
    mkText(scoreTxt, font, 38, sf::Color::White, 0, 0);
    scoreTxt.setOutlineColor(sf::Color::Black);
    scoreTxt.setOutlineThickness(3);

    mkText(levelTxt, font, 22, sf::Color(220, 255, 180), 10, 50);
    mkText(goTxt, font, 54, sf::Color::White, 0, 0, "GAME  OVER");
    goTxt.setOutlineColor(sf::Color::Black);
    goTxt.setOutlineThickness(4);

    mkText(restartTxt, font, 26, sf::Color(255, 230, 80), 0, 0,
        "Press  Enter  or  R  to restart");
    mkText(bestTxt, font, 24, sf::Color(255, 200, 100), 0, 0);

    // center texts
    auto centerX = [&](sf::Text& t, float y) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(WIN_W / 2.f, y);
        };
    centerX(goTxt, WIN_H / 2.f - 60.f);
    centerX(restartTxt, WIN_H / 2.f + 55.f);

    // Game-over panel
    sf::RectangleShape goPanel(sf::Vector2f(440.f, 200.f));
    goPanel.setFillColor(sf::Color(0, 0, 0, 170));
    goPanel.setOutlineColor(sf::Color(255, 200, 80, 200));
    goPanel.setOutlineThickness(2.f);
    goPanel.setOrigin(220.f, 100.f);
    goPanel.setPosition(WIN_W / 2.f, WIN_H / 2.f);

    // Flash overlay
    sf::RectangleShape flashRect(sf::Vector2f(WIN_W, WIN_H));
    flashRect.setFillColor(sf::Color::White);

    // Ground overlay strip
    sf::RectangleShape groundStrip(sf::Vector2f(WIN_W, WIN_H - GROUND_Y));
    groundStrip.setFillColor(sf::Color(110, 170, 50));
    groundStrip.setPosition(0, GROUND_Y);

    sf::RectangleShape groundLine(sf::Vector2f(WIN_W, 4.f));
    groundLine.setFillColor(sf::Color(70, 130, 30));
    groundLine.setPosition(0, GROUND_Y);

    int bestScore = 0;

    auto resetGame = [&]() {
        gameOver = false;
        hitPlayed = false;
        score = 0;
        pipesPassed = 0;
        pipeSpeed = INITIAL_PIPE_SPD;
        flashAlpha = 0.f;
        deathTimer = 0.f;
        pipes.clear();
        particles.clear();
        bird.reset();
        fillPipes(pipes, pt);
        };

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        //Events 
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) window.close();

            if (ev.type == sf::Event::KeyPressed)
            {
                if (!gameOver)
                {
                    if (ev.key.code == sf::Keyboard::Space ||
                        ev.key.code == sf::Keyboard::Up)
                    {
                        bird.flap();
                        audio.flap();
                    }
                }
                else
                {
                    if (ev.key.code == sf::Keyboard::Enter ||
                        ev.key.code == sf::Keyboard::R)
                        resetGame();
                }
            }
            if (ev.type == sf::Event::MouseButtonPressed && !gameOver)
            {
                bird.flap();
                audio.flap();
            }
        }

        // Update 
        if (!gameOver)
        {
            groundScroll += pipeSpeed * dt;
            if (groundScroll > 32.f) groundScroll -= 32.f;

            bird.update(dt);

            // Pipes
            for (int i = (int)pipes.size() - 1; i >= 0; --i)
            {
                pipes[i].update(pipeSpeed, dt);

                // Score
                float pRight = pipes[i].x + PIPE_WIDTH;
                if (!pipes[i].counted && pRight < bird.pos.x)
                {
                    pipes[i].counted = true;
                    score++;
                    pipesPassed++;
                    audio.score();

                    // Spawn score particles
                    for (int p = 0; p < 8; ++p) {
                        float angle2 = (std::rand() % 360) * 3.14159f / 180.f;
                        float spd2 = 50 + std::rand() % 100;
                        Particle par;
                        par.shape.setRadius(4);
                        par.shape.setFillColor(sf::Color(255, 220, 50, 255));
                        par.shape.setPosition(bird.pos.x, bird.pos.y - 20);
                        par.vel = { std::cos(angle2) * spd2, std::sin(angle2) * spd2 };
                        par.life = 1.f;
                        particles.push_back(par);
                    }

                    // Level up
                    if (pipesPassed % LEVEL_EVERY == 0) {
                        pipeSpeed += SPEED_INCREMENT;
                        audio.levelup();
                    }
                }

                // Collision
                if (pipes[i].collides(bird.bounds())) {
                    gameOver = true;
                    bird.dead = true;
                    flashAlpha = 255.f;
                    audio.hit();
                    hitPlayed = true;
                }

                if (pipes[i].isOffScreen())
                    pipes.erase(pipes.begin() + i);
            }

            fillPipes(pipes, pt);

            // Out of bounds
            if (bird.outOfBounds()) {
                gameOver = true;
                bird.dead = true;
                flashAlpha = 255.f;
                if (!hitPlayed) { audio.hit(); hitPlayed = true; }
            }

            // Particles
            for (int i = (int)particles.size() - 1; i >= 0; --i) {
                auto& p = particles[i];
                p.shape.move(p.vel * dt);
                p.vel.y += 200.f * dt;
                p.life -= dt * 2.f;
                if (p.life <= 0) { particles.erase(particles.begin() + i); continue; }
                sf::Color c = p.shape.getFillColor();
                c.a = (sf::Uint8)(255 * std::max(0.f, p.life));
                p.shape.setFillColor(c);
            }
        }
        else
        {
            deathTimer += dt;
            // die sound after short delay
            if (deathTimer > 0.35f && !hitPlayed) { audio.die(); hitPlayed = true; }
            if (deathTimer > 0.4f && hitPlayed && deathTimer < 0.41f) audio.die();
        }

        // Fade flash
        if (flashAlpha > 0) flashAlpha = std::max(0.f, flashAlpha - 400.f * dt);

        // Score text
        {
            std::ostringstream ss;
            ss << score;
            scoreTxt.setString(ss.str());
            auto b = scoreTxt.getLocalBounds();
            scoreTxt.setOrigin(b.left + b.width / 2.f, b.top);
            scoreTxt.setPosition(WIN_W / 2.f, 12.f);
        }
        {
            int lvl = 1 + pipesPassed / LEVEL_EVERY;
            std::ostringstream ss;
            ss << "Level " << lvl << "   Speed: " << (int)pipeSpeed;
            levelTxt.setString(ss.str());
        }
        if (score > bestScore) bestScore = score;

        // ---- Render ----
        window.clear(sf::Color(100, 180, 240));
        window.draw(bgSprite);

        // Pipes
        for (auto& p : pipes) p.draw(window);

        // Ground
        window.draw(groundStrip);
        window.draw(groundLine);

        // Particles
        for (auto& p : particles) window.draw(p.shape);

        // Bird
        bird.draw(window);

        // HUD
        window.draw(levelTxt);
        window.draw(scoreTxt);

        // Flash
        if (flashAlpha > 0) {
            flashRect.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)flashAlpha));
            window.draw(flashRect);
        }

        // Game-over screen
        if (gameOver) {
            window.draw(goPanel);
            window.draw(goTxt);

            std::ostringstream ss;
            ss << "Score: " << score << "     Best: " << bestScore;
            bestTxt.setString(ss.str());
            centerX(bestTxt, WIN_H / 2.f - 5.f);
            window.draw(bestTxt);
            window.draw(restartTxt);
        }

        window.display();
    }
    return 0;
}
