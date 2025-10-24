--- CMakeLists.txt ---
cmake_minimum_required(VERSION 3.10)
project(TicTacToeSFML)
set(CMAKE_CXX_STANDARD 17)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
add_executable(tictactoe src/main.cpp)
target_include_directories(tictactoe PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tictactoe PRIVATE sfml-graphics sfml-window sfml-system)

--- README.md ---
# TicTacToe C++ (SFML) - OOP Project

This project is a tic-tac-toe game implemented in C++ using an OOP structure and a GUI built with SFML.

## Requirements
- C++17 compatible compiler
- CMake 3.10+
- SFML 2.5+ installed
- A TrueType font file placed in `assets/` (e.g. `assets/arial.ttf`)

## Build (Linux/macOS)
```bash
mkdir build && cd build
cmake ..
make
./tictactoe
```

## Notes
- The code is in a single-source file `src/main.cpp` for simplicity. You can split classes into headers/sources if desired.
- Place a TTF font inside `assets/` and name it `font.ttf` (or change the path in the code).

--- src/main.cpp ---
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <optional>
#include <limits>
#include <string>
#include <iostream>

// Simple TicTacToe with OOP and SFML GUI
// Classes: Board, AI, Game, GUI

enum class Cell { Empty, X, O };

class Board {
public:
    static const int SIZE = 3;
    Board() { cells.fill(Cell::Empty); }

    bool makeMove(int r, int c, Cell player) {
        int idx = r * SIZE + c;
        if (cells[idx] == Cell::Empty) {
            cells[idx] = player;
            return true;
        }
        return false;
    }

    void undoMove(int r, int c) {
        cells[r * SIZE + c] = Cell::Empty;
    }

    std::vector<std::pair<int,int>> availableMoves() const {
        std::vector<std::pair<int,int>> moves;
        for (int r = 0; r < SIZE; ++r) for (int c = 0; c < SIZE; ++c)
            if (cells[r * SIZE + c] == Cell::Empty) moves.emplace_back(r,c);
        return moves;
    }

    bool isFull() const {
        for (auto &v: cells) if (v == Cell::Empty) return false;
        return true;
    }

    std::optional<Cell> checkWinner() const {
        // rows
        for (int r=0;r<SIZE;++r) {
            if (cells[r*SIZE] != Cell::Empty && cells[r*SIZE] == cells[r*SIZE+1] && cells[r*SIZE] == cells[r*SIZE+2])
                return cells[r*SIZE];
        }
        // cols
        for (int c=0;c<SIZE;++c) {
            if (cells[c] != Cell::Empty && cells[c] == cells[SIZE + c] && cells[c] == cells[2*SIZE + c])
                return cells[c];
        }
        // diagonals
        if (cells[0] != Cell::Empty && cells[0] == cells[4] && cells[0] == cells[8]) return cells[0];
        if (cells[2] != Cell::Empty && cells[2] == cells[4] && cells[2] == cells[6]) return cells[2];
        return std::nullopt;
    }

    Cell get(int r, int c) const { return cells[r*SIZE + c]; }

    void reset() { cells.fill(Cell::Empty); }

private:
    std::array<Cell, SIZE*SIZE> cells;
};

// MiniMax AI for TicTacToe
class AI {
public:
    AI(Cell aiPlayer, Cell humanPlayer): ai(aiPlayer), human(humanPlayer) {}

    // returns best move (r,c)
    std::pair<int,int> findBestMove(Board board) {
        int bestScore = std::numeric_limits<int>::min();
        std::pair<int,int> bestMove = {-1,-1};
        for (auto m: board.availableMoves()) {
            board.makeMove(m.first, m.second, ai);
            int score = minimax(board, 0, false);
            board.undoMove(m.first, m.second);
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
        }
        return bestMove;
    }

private:
    Cell ai, human;

    int scoreForWinner(Cell winner) const {
        if (winner == ai) return 10;
        if (winner == human) return -10;
        return 0;
    }

    int minimax(Board &board, int depth, bool isMaximizing) {
        auto winner = board.checkWinner();
        if (winner.has_value()) return scoreForWinner(*winner);
        if (board.isFull()) return 0;

        if (isMaximizing) {
            int best = std::numeric_limits<int>::min();
            for (auto m: board.availableMoves()) {
                board.makeMove(m.first, m.second, ai);
                best = std::max(best, minimax(board, depth+1, false));
                board.undoMove(m.first, m.second);
            }
            return best;
        } else {
            int best = std::numeric_limits<int>::max();
            for (auto m: board.availableMoves()) {
                board.makeMove(m.first, m.second, human);
                best = std::min(best, minimax(board, depth+1, true));
                board.undoMove(m.first, m.second);
            }
            return best;
        }
    }
};

class Game {
public:
    enum class Mode { HumanVsHuman, HumanVsAI };
    Game(): board(), current(Cell::X), mode(Mode::HumanVsAI), ai(Cell::O, Cell::X), scoreX(0), scoreO(0) {}

    void restart() {
        board.reset();
        current = Cell::X;
        over = false;
        winnerOpt.reset();
    }

    bool playMove(int r, int c) {
        if (over) return false;
        if (board.get(r,c) != Cell::Empty) return false;
        board.makeMove(r,c,current);
        checkGameState();
        if (!over) switchTurn();
        return true;
    }

    void aiMove() {
        if (over) return;
        auto [r,c] = ai.findBestMove(board);
        if (r>=0) {
            board.makeMove(r,c, Cell::O);
            checkGameState();
            if (!over) switchTurn();
        }
    }

    void setMode(Mode m) { mode = m; }
    Mode getMode() const { return mode; }
    Cell currentPlayer() const { return current; }
    Board const& getBoard() const { return board; }
    bool isOver() const { return over; }
    std::optional<Cell> winner() const { return winnerOpt; }
    int getScoreX() const { return scoreX; }
    int getScoreO() const { return scoreO; }

private:
    Board board;
    Cell current;
    Mode mode;
    AI ai{Cell::O, Cell::X};
    bool over = false;
    std::optional<Cell> winnerOpt;
    int scoreX;
    int scoreO;

    void switchTurn() { current = (current==Cell::X?Cell::O:Cell::X); }

    void checkGameState() {
        auto w = board.checkWinner();
        if (w.has_value()) {
            over = true;
            winnerOpt = w;
            if (*w == Cell::X) ++scoreX; else ++scoreO;
        } else if (board.isFull()) {
            over = true;
            winnerOpt = std::nullopt; // draw
        }
    }
};

// GUI wrapper using SFML
class GUI {
public:
    GUI(Game &game): game(game), window(sf::VideoMode(600,700), "TicTacToe - C++ SFML") {
        if (!font.loadFromFile("assets/font.ttf")) {
            std::cerr << "Warning: failed to load assets/font.ttf. Text may not display.\n";
        }
        cellSize = 600 / Board::SIZE; // square
        gridOffset = {0,0};
        setupShapes();
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    Game &game;
    sf::RenderWindow window;
    sf::Font font;
    int cellSize;
    sf::Vector2f gridOffset;
    sf::RectangleShape lines[4];
    sf::Color bgColor = sf::Color(30,30,30);

    void setupShapes() {
        float thickness = 4.f;
        // vertical lines
        for (int i=0;i<2;++i) {
            lines[i].setSize(sf::Vector2f(thickness, (float)600));
            lines[i].setPosition((i+1)*cellSize - thickness/2.f, 0.f);
            lines[i].setFillColor(sf::Color::White);
        }
        // horizontal lines
        for (int i=0;i<2;++i) {
            lines[2+i].setSize(sf::Vector2f((float)600, thickness));
            lines[2+i].setPosition(0.f, (i+1)*cellSize - thickness/2.f);
            lines[2+i].setFillColor(sf::Color::White);
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int mx = event.mouseButton.x;
                    int my = event.mouseButton.y;
                    if (my <= 600) {
                        int r = my / cellSize;
                        int c = mx / cellSize;
                        if (game.getMode() == Game::Mode::HumanVsHuman) {
                            game.playMove(r,c);
                        } else {
                            if (game.currentPlayer() == Cell::X) {
                                if (game.playMove(r,c)) {
                                    // AI turn
                                    if (!game.isOver()) game.aiMove();
                                }
                            }
                        }
                    } else {
                        // bottom area for buttons
                        handleBottomClick(mx, my);
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) game.restart();
                if (event.key.code == sf::Keyboard::M) toggleMode();
            }
        }
    }

    void handleBottomClick(int mx, int my) {
        // simple clickable zones: Restart and Toggle Mode
        // Restart button centered bottom
        int y = 620;
        if (mx >= 50 && mx <= 250 && my >= 610 && my <= 660) {
            game.restart();
        }
        if (mx >= 350 && mx <= 550 && my >= 610 && my <= 660) {
            toggleMode();
        }
    }

    void toggleMode() {
        if (game.getMode() == Game::Mode::HumanVsHuman) game.setMode(Game::Mode::HumanVsAI);
        else game.setMode(Game::Mode::HumanVsHuman);
        game.restart();
    }

    void update() {
        // If AI vs Human and it's AI's turn, make AI move (safeguarded to not busy loop)
        if (game.getMode() == Game::Mode::HumanVsAI && !game.isOver() && game.currentPlayer() == Cell::O) {
            // small delay to improve UX
            static sf::Clock cooldown; static bool started = false;
            if (!started) { cooldown.restart(); started = true; }
            if (cooldown.getElapsedTime().asMilliseconds() > 300) {
                game.aiMove();
                started = false;
            }
        }
    }

    void render() {
        window.clear(bgColor);
        drawGrid();
        drawMarks();
        drawBottomUI();
        window.display();
    }

    void drawGrid() {
        for (int i=0;i<4;++i) window.draw(lines[i]);
    }

    void drawMarks() {
        Board const &board = game.getBoard();
        for (int r=0;r<Board::SIZE;++r) for (int c=0;c<Board::SIZE;++c) {
            Cell cell = board.get(r,c);
            float x = c * cellSize;
            float y = r * cellSize;
            if (cell == Cell::X) drawX(x,y);
            else if (cell == Cell::O) drawO(x,y);
        }
        if (game.isOver()) {
            sf::RectangleShape overlay(sf::Vector2f(600,600));
            overlay.setFillColor(sf::Color(0,0,0,120));
            window.draw(overlay);
            drawGameOver();
        }
    }

    void drawX(float x, float y) {
        float pad = cellSize * 0.2f;
        sf::RectangleShape l1(sf::Vector2f(cellSize - 2*pad, 6.f));
        l1.setOrigin(0, 3.f);
        l1.setPosition(x + pad, y + pad);
        l1.setRotation(45.f);
        l1.setFillColor(sf::Color::Red);

        sf::RectangleShape l2(sf::Vector2f(cellSize - 2*pad, 6.f));
        l2.setOrigin(0, 3.f);
        l2.setPosition(x + cellSize - pad, y + pad);
        l2.setRotation(135.f);
        l2.setFillColor(sf::Color::Red);

        window.draw(l1);
        window.draw(l2);
    }

    void drawO(float x, float y) {
        float pad = cellSize * 0.18f;
        sf::CircleShape circle((cellSize - 2*pad)/2.f);
        circle.setPosition(x + pad, y + pad);
        circle.setOutlineThickness(6.f);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color::Cyan);
        window.draw(circle);
    }

    void drawBottomUI() {
        // Draw separator
        sf::RectangleShape sep(sf::Vector2f(600.f, 2.f));
        sep.setPosition(0,600);
        sep.setFillColor(sf::Color::White);
        window.draw(sep);

        // Restart button
        sf::RectangleShape restartBtn(sf::Vector2f(200,50));
        restartBtn.setPosition(50,620);
        restartBtn.setFillColor(sf::Color(80,80,80));
        window.draw(restartBtn);
        drawText("Restart (R)", 60, 630, 20);

        // Toggle Mode button
        sf::RectangleShape modeBtn(sf::Vector2f(200,50));
        modeBtn.setPosition(350,620);
        modeBtn.setFillColor(sf::Color(80,80,80));
        window.draw(modeBtn);
        std::string modeText = (game.getMode()==Game::Mode::HumanVsHuman)?"Human vs Human (M)":"Human vs AI (M)";
        drawText(modeText, 360, 630, 18);

        // Score
        std::string scoreStr = "X: " + std::to_string(game.getScoreX()) + "    O: " + std::to_string(game.getScoreO());
        drawText(scoreStr, 250, 610, 16);

        // Current turn
        std::string turnText;
        if (!game.isOver()) {
            turnText = (game.currentPlayer()==Cell::X)?"Turn: X":"Turn: O";
        } else {
            if (game.winner().has_value()) {
                turnText = (*game.winner()==Cell::X)?"Winner: X":"Winner: O";
            } else turnText = "Draw";
        }
        drawText(turnText, 10, 580, 16);
    }

    void drawText(const std::string &text, float x, float y, unsigned int size) {
        if (!font.getInfo().family.empty()) {
            sf::Text t(text, font, size);
            t.setPosition(x,y);
            t.setFillColor(sf::Color::White);
            window.draw(t);
        } else {
            // no font loaded; skip drawing text
        }
    }

    void drawGameOver() {
        std::string t;
        if (game.winner().has_value()) {
            t = (*game.winner()==Cell::X)?"X Wins!":"O Wins!";
        } else t = "Draw!";
        if (!font.getInfo().family.empty()) {
            sf::Text txt(t, font, 48);
            txt.setFillColor(sf::Color::Yellow);
            sf::FloatRect bb = txt.getLocalBounds();
            txt.setPosition((600 - bb.width)/2.f, (600 - bb.height)/2.f - 20);
            window.draw(txt);

            sf::Text sub("Click Restart or press R to play again", font, 16);
            sub.setFillColor(sf::Color::White);
            sf::FloatRect sbb = sub.getLocalBounds();
            sub.setPosition((600 - sbb.width)/2.f, (600 - sbb.height)/2.f + 30);
            window.draw(sub);
        }
    }
};

int main() {
    Game game;
    GUI gui(game);
    gui.run();
    return 0;
}

--- assets/README.txt ---
Place a TTF font in this folder and name it `font.ttf` (or change the path in the code to point to your font file).

--- End of files ---
