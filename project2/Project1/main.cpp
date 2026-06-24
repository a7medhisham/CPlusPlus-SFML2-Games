#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

enum class Player { None, X, O };

// Audio Manager
struct AudioManager
{
    sf::SoundBuffer bufPlaceX, bufPlaceO, bufWin, bufDraw;
    sf::Sound       sndPlaceX, sndPlaceO, sndWin, sndDraw;

    void load()
    {
        auto tryLoad = [](sf::SoundBuffer& buf, const string& path) {
            if (!buf.loadFromFile(path))
                if (!buf.loadFromFile("assets/" + path))
                    cerr << "Audio not found: " << path << "\n";
            };

        tryLoad(bufPlaceX, "place_x.wav");
        tryLoad(bufPlaceO, "place_o.wav");
        tryLoad(bufWin, "win.wav");
        tryLoad(bufDraw, "draw.wav");

        sndPlaceX.setBuffer(bufPlaceX);
        sndPlaceO.setBuffer(bufPlaceO);
        sndWin.setBuffer(bufWin);
        sndDraw.setBuffer(bufDraw);
    }

    void playPlaceX() { sndPlaceX.play(); }
    void playPlaceO() { sndPlaceO.play(); }
    void playWin() { sndWin.play(); }
    void playDraw() { sndDraw.play(); }
};

// TicTacToe
class TicTacToe
{
public:
    Player board[3][3] =
    {
        { Player::None, Player::None, Player::None },
        { Player::None, Player::None, Player::None },
        { Player::None, Player::None, Player::None }
    };

    TicTacToe()
    {
        window.create(sf::VideoMode(300, 300), "Tic-Tac-Toe");
        window.setFramerateLimit(60);
        currentPlayer = Player::X;

        if (!font.loadFromFile("arial.ttf"))
            if (!font.loadFromFile("assets/arial.ttf"))
                cout << "Failed to load font!\n";

        audio.load();
    }

    void run()
    {
        while (window.isOpen())
        {
            handleEvents();
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Font         font;
    AudioManager     audio;

    Player currentPlayer;
    Player winner = Player::None;
    bool   gameOver = false;

    // Handle Events
    void handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (!gameOver &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    int row = event.mouseButton.y / 100;
                    int col = event.mouseButton.x / 100;

                    if (row >= 0 && row < 3 &&
                        col >= 0 && col < 3 &&
                        board[row][col] == Player::None)
                    {
                        board[row][col] = currentPlayer;

                        // --- play placement sound ---
                        if (currentPlayer == Player::X)
                            audio.playPlaceX();
                        else
                            audio.playPlaceO();

                        if (checkWin())
                        {
                            winner = currentPlayer;
                            gameOver = true;
                            audio.playWin();   // win sound 

                            cout << (currentPlayer == Player::X ?
                                "Player X" : "Player O")
                                << " wins!\n";
                        }
                        else if (checkDraw())
                        {
                            gameOver = true;
                            audio.playDraw();  // draw sound 
                            cout << "It's a draw!\n";
                        }
                        else
                        {
                            currentPlayer =
                                (currentPlayer == Player::X)
                                ? Player::O : Player::X;
                        }
                    }
                }

                // Click anywhere on end-screen to restart
                if (gameOver)
                    resetGame();
            }
        }
    }
    // Reset
    void resetGame()
    {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                board[r][c] = Player::None;

        currentPlayer = Player::X;
        winner = Player::None;
        gameOver = false;
    }
    // Render
    void render()
    {
        window.clear(sf::Color::White);

        // Grid
        for (int i = 1; i < 3; ++i)
        {
            sf::RectangleShape hLine(sf::Vector2f(300.f, 4.f));
            hLine.setFillColor(sf::Color::Black);
            hLine.setPosition(0.f, i * 100.f - 2.f);
            window.draw(hLine);

            sf::RectangleShape vLine(sf::Vector2f(4.f, 300.f));
            vLine.setFillColor(sf::Color::Black);
            vLine.setPosition(i * 100.f - 2.f, 0.f);
            window.draw(vLine);
        }

        // Symbols
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
            {
                if (board[row][col] == Player::X)
                    drawSymbol("X", row, col, sf::Color::Red);
                else if (board[row][col] == Player::O)
                    drawSymbol("O", row, col, sf::Color::Blue);
            }

        if (gameOver)
            drawEndScreen();

        window.display();
    }
    // Draw Symbol
    void drawSymbol(const string& sym, int row, int col, sf::Color color)
    {
        sf::Text text;
        text.setFont(font);
        text.setString(sym);
        text.setCharacterSize(80);
        text.setFillColor(color);
        text.setPosition(col * 100.f + 18.f, row * 100.f + 8.f);
        window.draw(text);
    }
    // Draw End Screen
    void drawEndScreen()
    {
        sf::RectangleShape overlay(sf::Vector2f(300.f, 80.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 170));
        overlay.setPosition(0.f, 110.f);
        window.draw(overlay);

        string msg;
        if (winner == Player::X)      msg = "Player X Wins!";
        else if (winner == Player::O) msg = "Player O Wins!";
        else                          msg = "Draw!";

        sf::Text text;
        text.setFont(font);
        text.setString(msg);
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::White);
        sf::FloatRect b = text.getLocalBounds();
        text.setOrigin(b.width / 2.f, b.height / 2.f);
        text.setPosition(150.f, 143.f);
        window.draw(text);

        // "Click to restart" hint
        sf::Text hint;
        hint.setFont(font);
        hint.setString("Click to restart");
        hint.setCharacterSize(16);
        hint.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
        hint.setPosition(150.f, 172.f);
        window.draw(hint);
    }

    // Check Win
    bool checkWin()
    {
        for (int i = 0; i < 3; ++i)
        {
            if (board[i][0] != Player::None &&
                board[i][0] == board[i][1] &&
                board[i][1] == board[i][2]) return true;

            if (board[0][i] != Player::None &&
                board[0][i] == board[1][i] &&
                board[1][i] == board[2][i]) return true;
        }

        if (board[0][0] != Player::None &&
            board[0][0] == board[1][1] &&
            board[1][1] == board[2][2]) return true;

        if (board[0][2] != Player::None &&
            board[0][2] == board[1][1] &&
            board[1][1] == board[2][0]) return true;

        return false;
    }

    // Check Draw
    bool checkDraw()
    {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                if (board[r][c] == Player::None)
                    return false;
        return true;
    }
};

int main()
{
    TicTacToe game;
    game.run();
    return 0;
}
