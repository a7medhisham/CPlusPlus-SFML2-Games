#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <deque>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        "Bouncing Ball"
    );

    window.setFramerateLimit(60);

    // Load Bounce Sound
    sf::Music music;

    if (!music.openFromFile("ballbounce2.mp3"))
        return -1;

    // Load Game Over Sound
    sf::SoundBuffer gameOverBuffer;

    if (!gameOverBuffer.loadFromFile("gameover2.mp3"))
    {
        std::cout << "Failed to load game over sound!\n";
    }

    sf::Sound gameOverSound;
    gameOverSound.setBuffer(gameOverBuffer);

    // Background Gradient
    sf::VertexArray background(sf::Quads, 4);
    background[0] = { {0.f,   0.f},   sf::Color(5,  5,  20) };
    background[1] = { {800.f, 0.f},   sf::Color(10, 5,  30) };
    background[2] = { {800.f, 600.f}, sf::Color(0,  0,  10) };
    background[3] = { {0.f,   600.f}, sf::Color(5,  0,  15) };

    // Ball Setup (Core + Glow)
    sf::CircleShape glowBall(38.f);
    glowBall.setFillColor(sf::Color(0, 100, 255, 55));
    glowBall.setOrigin(38.f, 38.f);

    sf::CircleShape ball(20.f);
    ball.setFillColor(sf::Color(80, 160, 255));
    ball.setOrigin(20.f, 20.f);
    ball.setPosition(400.f, 300.f);

    // Trail Effect
    std::deque<sf::Vector2f> trail;
    const int TRAIL_LEN = 14;
    // Speed Constants  (pixels/sec, dt-based)
    const float BASE_SPEED = 220.f;
    const float MAX_SPEED = 750.f;
    const float SPEED_STEP = 0.15f;

    float currentSpeed = BASE_SPEED;

    sf::Vector2f velocity(currentSpeed, currentSpeed);
    // Paddle Constants
    const float PADDLE_START_WIDTH = 180.f;
    const float PADDLE_MIN_WIDTH = 60.f;
    const float PADDLE_SHRINK = 20.f;

    sf::VertexArray paddleVA(sf::Quads, 4);

    sf::RectangleShape paddle(sf::Vector2f(PADDLE_START_WIDTH, 18.f));
    paddle.setPosition(310.f, 555.f);

    const float PADDLE_SPEED = 600.f;

    float hitFlash = 0.f;
    const float FLASH_DUR = 0.12f;

    // Helper: rebuild paddle gradient vertex array
    auto rebuildPaddleVA = [&]()
        {
            auto pos = paddle.getPosition();
            auto sz = paddle.getSize();

            paddleVA[0] = { {pos.x,        pos.y},        sf::Color(0, 200, 255) };
            paddleVA[1] = { {pos.x + sz.x, pos.y},        sf::Color(0, 255, 150) };
            paddleVA[2] = { {pos.x + sz.x, pos.y + sz.y}, sf::Color(0, 200, 120) };
            paddleVA[3] = { {pos.x,        pos.y + sz.y}, sf::Color(0, 150, 200) };

            // Apply hit flash (brighten toward white)
            if (hitFlash > 0.f)
            {
                float t = hitFlash / FLASH_DUR;
                sf::Uint8 boost = static_cast<sf::Uint8>(255 * t);
                for (int i = 0; i < 4; ++i)
                {
                    paddleVA[i].color.r = std::min(255, (int)paddleVA[i].color.r + boost);
                    paddleVA[i].color.g = std::min(255, (int)paddleVA[i].color.g + boost);
                    paddleVA[i].color.b = std::min(255, (int)paddleVA[i].color.b + boost);
                }
            }
        };

    // Score Variables
    int score = 0;
    int lastMilestone = 0;
    bool paddleHit = false;
    // Font
    sf::Font font;

    if (!font.loadFromFile("arial.ttf"))
        return -1;
    // Score Text
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Score : 0");
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::Cyan);
    scoreText.setPosition(10.f, 10.f);

    // Speed Text
    sf::Text speedText;
    speedText.setFont(font);
    speedText.setString("Speed Lv: 1");
    speedText.setCharacterSize(24);
    speedText.setFillColor(sf::Color::Yellow);
    speedText.setPosition(10.f, 50.f);
    // Next Level Text
    sf::Text nextLevelText;
    nextLevelText.setFont(font);
    nextLevelText.setCharacterSize(22);
    nextLevelText.setFillColor(sf::Color(255, 200, 0));
    nextLevelText.setPosition(10.f, 80.f);
    // Game Over Text
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);

    sf::FloatRect goRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(
        goRect.left + goRect.width / 2.f,
        goRect.top + goRect.height / 2.f
    );
    gameOverText.setPosition(400.f, 260.f);

    // Restart Text
    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("Press R to Restart");
    restartText.setCharacterSize(28);
    restartText.setFillColor(sf::Color::White);

    sf::FloatRect restartRect = restartText.getLocalBounds();
    restartText.setOrigin(
        restartRect.left + restartRect.width / 2.f,
        restartRect.top + restartRect.height / 2.f
    );
    restartText.setPosition(400.f, 340.f);

    // Game State
    bool gameOver = false;

    sf::Clock clock;

    // Game Loop
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        // clamp dt so a lag spike doesn't teleport the ball
        if (dt > 0.05f) dt = 0.05f;

        // Events
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::R && gameOver)
                {
                    score = 0;
                    lastMilestone = 0;
                    currentSpeed = BASE_SPEED;
                    velocity = sf::Vector2f(BASE_SPEED, BASE_SPEED);
                    hitFlash = 0.f;

                    ball.setPosition(400.f, 300.f);

                    paddle.setSize(sf::Vector2f(PADDLE_START_WIDTH, 18.f));
                    paddle.setPosition(310.f, 555.f);

                    trail.clear();

                    scoreText.setString("Score : 0");
                    speedText.setString("Speed Lv: 1");
                    nextLevelText.setString("");

                    gameOver = false;
                    gameOverSound.stop();
                }
            }
        }

        // Update
        if (!gameOver)
        {
            // Move Paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                paddle.move(-PADDLE_SPEED * dt, 0.f);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                paddle.move(PADDLE_SPEED * dt, 0.f);
            }

            // Clamp paddle inside window
            if (paddle.getPosition().x < 0.f)
                paddle.setPosition(0.f, paddle.getPosition().y);

            if (paddle.getPosition().x + paddle.getSize().x > 800.f)
                paddle.setPosition(800.f - paddle.getSize().x, paddle.getPosition().y);

            // Move Ball (dt-based)
            ball.move(velocity * dt);

            // Update Trail
            trail.push_front(ball.getPosition());
            if ((int)trail.size() > TRAIL_LEN)
                trail.pop_back();
            // Bounce Left / Right
            if (ball.getPosition().x - ball.getRadius() <= 0.f)
            {
                velocity.x = std::abs(velocity.x);
                if (music.getStatus() != sf::Music::Playing)
                    music.play();
            }
            else if (ball.getPosition().x + ball.getRadius() >= 800.f)
            {
                velocity.x = -std::abs(velocity.x);
                if (music.getStatus() != sf::Music::Playing)
                    music.play();
            }
            // Bounce Top
            if (ball.getPosition().y - ball.getRadius() <= 0.f)
            {
                velocity.y = std::abs(velocity.y);
                if (music.getStatus() != sf::Music::Playing)
                    music.play();
            }
            // Paddle Collision
            sf::FloatRect ballBounds(
                ball.getPosition().x - ball.getRadius(),
                ball.getPosition().y - ball.getRadius(),
                ball.getRadius() * 2.f,
                ball.getRadius() * 2.f
            );

            bool touching = ballBounds.intersects(paddle.getGlobalBounds());

            if (touching && !paddleHit)
            {
                paddleHit = true;
                hitFlash = FLASH_DUR;

                // Angle based on hit position on paddle
                float hitPos = (ball.getPosition().x - paddle.getPosition().x)
                    / paddle.getSize().x;
                hitPos = std::max(0.f, std::min(1.f, hitPos));

                float angle = (hitPos - 0.5f) * 140.f; // -70 to +70 degrees
                float rad = angle * 3.14159f / 180.f;

                velocity.x = currentSpeed * std::sin(rad);
                velocity.y = -currentSpeed * std::cos(rad);

                // Prevent too-shallow vertical bounce
                if (std::abs(velocity.y) < currentSpeed * 0.3f)
                    velocity.y = -currentSpeed * 0.3f;

                if (music.getStatus() != sf::Music::Playing)
                    music.play();

                // Increase Score
                score++;
                scoreText.setString("Score : " + std::to_string(score));

                // Every 5 Points
                if (score - lastMilestone >= 5)
                {
                    lastMilestone = score;

                    if (currentSpeed < MAX_SPEED)
                    {
                        currentSpeed = std::min(
                            currentSpeed * (1.f + SPEED_STEP),
                            MAX_SPEED
                        );
                    }

                    // Shrink Paddle
                    float newWidth = std::max(
                        paddle.getSize().x - PADDLE_SHRINK,
                        PADDLE_MIN_WIDTH
                    );
                    paddle.setSize(sf::Vector2f(newWidth, 18.f));

                    // Clamp paddle after shrink
                    if (paddle.getPosition().x + newWidth > 800.f)
                        paddle.setPosition(800.f - newWidth, paddle.getPosition().y);

                    int level = score / 5;
                    speedText.setString("Speed Lv: " + std::to_string(level));
                    nextLevelText.setString(
                        "Next level at " + std::to_string((level + 1) * 5)
                    );
                }
            }

            if (!touching)
                paddleHit = false;

            // Hit Flash Decay
            if (hitFlash > 0.f)
                hitFlash -= dt;
            // Game Over
            if (ball.getPosition().y + ball.getRadius() >= 600.f)
            {
                gameOver = true;
                trail.clear();

                if (music.getStatus() != sf::Music::Playing)
                    music.play();

                gameOverSound.play();
            }
        }
        // Draw
        window.clear();

        // Background
        window.draw(background);

        // Trail
        {
            sf::CircleShape ghost(ball.getRadius());
            ghost.setOrigin(ball.getRadius(), ball.getRadius());
            for (int i = 0; i < (int)trail.size(); ++i)
            {
                float alpha = 110.f * (1.f - float(i) / TRAIL_LEN);
                ghost.setFillColor(sf::Color(80, 160, 255, (sf::Uint8)alpha));
                ghost.setPosition(trail[i]);
                window.draw(ghost);
            }
        }

        // Glow ball
        glowBall.setPosition(ball.getPosition());
        window.draw(glowBall);

        // Core ball
        window.draw(ball);

        // Paddle (gradient vertex array)
        rebuildPaddleVA();
        window.draw(paddleVA);

        // HUD
        window.draw(scoreText);
        window.draw(speedText);
        window.draw(nextLevelText);

        // Game Over overlay
        if (gameOver)
        {
            window.draw(gameOverText);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}