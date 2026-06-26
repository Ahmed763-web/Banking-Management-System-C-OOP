#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;


const int GridSize = 9;
const int CandySize = 70;
const Vector2f WindowSize(900, 750);
int Score = 0, TargetScore = 5000, Moves = 20;
int GameGrid[GridSize][GridSize];



enum GameState { MENU, PLAYING, GAME_OVER, PAUSED, WINNER };
GameState currentState = MENU;

enum CandyType {
    EMPTY = 0,
    RED = 1, YELLOW, GREEN, BLUE, ORANGE,
    STRIPED_RED_H, STRIPED_YELLOW_H, STRIPED_GREEN_H, STRIPED_BLUE_H, STRIPED_ORANGE_H,
    STRIPED_RED_V, STRIPED_YELLOW_V, STRIPED_GREEN_V, STRIPED_BLUE_V, STRIPED_ORANGE_V,
    COLOR_BOMB, WRAPPED_RED, WRAPPED_YELLOW, WRAPPED_GREEN, WRAPPED_BLUE, WRAPPED_ORANGE
};

// candies ko initialize karwaata
struct GameAssets {
    Texture red, yellow, green, blue, orange;
    Texture stripedH[5], stripedV[5];
    Texture colorBomb;
    Texture background, menuBg;
    Texture wrapped[5];
    Font font;

    GameAssets() {
        red.loadFromFile("Resources/red.png");
        yellow.loadFromFile("Resources/yellow.png");
        green.loadFromFile("Resources/green.png");
        blue.loadFromFile("Resources/blue.png");
        orange.loadFromFile("Resources/orange.png");

        const string colors[] = { "red", "yellow", "green", "blue", "orange" };
        for (int i = 0; i < 5; i++) {
            stripedH[i].loadFromFile("Resources/striped_" + colors[i] + ".png");
            stripedV[i].loadFromFile("Resources/striped_" + colors[i] + ".png");
            wrapped[i].loadFromFile("Resources/wrapped_" + colors[i] + ".png");
        }
        colorBomb.loadFromFile("Resources/number1.png");
        background.loadFromFile("Resources/Main menu.jpg");
        menuBg.loadFromFile("Resources/Main menu.jpg");
        font.loadFromFile("Resources/arial.ttf");
    }
};

//  JAB AAP candy select karte ho to ye woh show karta hai uska border yellow rang mei
struct Selection {
    int x = -1, y = -1;
    RectangleShape selector;
    Selection() {
        selector.setSize(Vector2f(CandySize, CandySize));
        selector.setFillColor(Color::Transparent);
        selector.setOutlineThickness(3);
        selector.setOutlineColor(Color::Yellow);
    }
} selection;


void SaveGame() {
    ofstream saveFile("savegame.dat", ios::binary);
    if (!saveFile.is_open()) {
        cerr << "ERROR: Couldn't create save file!\n";
        return;
    }

    // ye grid state ko save karta
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y < GridSize; y++) {
            if (!(saveFile << GameGrid[x][y] << " ")) {
                cerr << "ERROR: Failed to write grid data!\n";
                saveFile.close();
                return;
            }
        }
    }

    //this mr.save file ka kaam hai to save the score
    if (!(saveFile << Score << " " << Moves)) {
        cerr << "ERROR: Failed to write game state!\n";
    }

    saveFile.close();
    cout << "Game saved successfully!\n";
}

void LoadGame() {
    ifstream saveFile("savegame.dat");
    if (saveFile.is_open()) {
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                if (!(saveFile >> GameGrid[x][y])) break;
            }
        }
        if (!(saveFile >> Score >> Moves)) {
            Score = 0;
            Moves = 20;
        }
        saveFile.close();
    }
}
// ye texts ko initiate karwaata like title hogaya play gameover retry buttons basically buttons ko and menu background waghera
void DrawGrid(RenderWindow& window, GameAssets& assets) {
    Sprite background((currentState == MENU || currentState == GAME_OVER) ? assets.menuBg : assets.background);
    background.setScale(
        WindowSize.x / background.getLocalBounds().width,
        WindowSize.y / background.getLocalBounds().height
    );
    window.draw(background);

    if (currentState == MENU) {


        RectangleShape playButton(Vector2f(200, 60));
        playButton.setPosition(WindowSize.x / 2 - 100, WindowSize.y / 2 + 115);
        playButton.setFillColor(Color::Green);

        Text playtext("Play", assets.font, 40);
        playtext.setPosition(WindowSize.x / 2 - 45, WindowSize.y / 2 + 120);

        // Load Button
        RectangleShape loadButton(Vector2f(200, 60));
        loadButton.setPosition(WindowSize.x / 2 - 100, WindowSize.y / 2 + 180);
        loadButton.setFillColor(Color::Red);

        Text loadText("Load Game", assets.font, 30);
        loadText.setPosition(WindowSize.x / 2 - 80, WindowSize.y / 2 + 190);


        window.draw(playButton);
        window.draw(playtext);
        window.draw(loadButton);
        window.draw(loadText);
    }

    else if (currentState == GAME_OVER) {


        Text scoreText("Final Score: " + to_string(Score), assets.font, 40);
        scoreText.setPosition(WindowSize.x / 2 - 120, WindowSize.y / 2 - 50);

        Text resultText("", assets.font, 60);
        resultText.setPosition(WindowSize.x / 2 - 180, WindowSize.y / 2 - 150);
        if (Score >= TargetScore) {
            resultText.setString("You Win!");
            resultText.setFillColor(Color::Green);
        }
        else {
            resultText.setString("Game Over!");
            resultText.setFillColor(Color::Red);

        }

        RectangleShape retryButton(Vector2f(200, 60));
        retryButton.setPosition(WindowSize.x / 2 - 100, WindowSize.y / 2 + 30);
        retryButton.setFillColor(Color::Blue);

        Text retryText("Retry", assets.font, 40);
        retryText.setPosition(WindowSize.x / 2 - 55, WindowSize.y / 2 + 35);

        window.draw(resultText);
        window.draw(scoreText);
        window.draw(retryButton);
        window.draw(retryText);
    }
    else if (currentState == WINNER) {
        Text winnerText("You Win!", assets.font, 60u);
        winnerText.setPosition(WindowSize.x / 2 - 120, WindowSize.y / 2 - 150);
        winnerText.setFillColor(Color::Green);

        Text scoreText("Final Score: " + to_string(Score), assets.font, 40u);
        scoreText.setPosition(WindowSize.x / 2 - 120, WindowSize.y / 2 - 50);

        RectangleShape retryButton(Vector2f(200, 60));
        retryButton.setPosition(WindowSize.x / 2 - 100, WindowSize.y / 2 + 30);
        retryButton.setFillColor(Color::Blue);

        Text retryText("Retry", assets.font, 40u);
        retryText.setPosition(WindowSize.x / 2 - 55, WindowSize.y / 2 + 35);

        window.draw(winnerText);
        window.draw(scoreText);
        window.draw(retryButton);
        window.draw(retryText);
    }

    if (currentState != PLAYING) return;

    const float startX = (WindowSize.x - GridSize * CandySize) / 2;
    const float startY = (WindowSize.y - GridSize * CandySize) / 2;
    //  BOARD KA BACKGROUND
    RectangleShape boardBackground(Vector2f(GridSize * CandySize, GridSize * CandySize));
    boardBackground.setPosition(startX, startY);
    boardBackground.setFillColor(Color(255, 255, 255, 100)); // Semi transparent white naya board
    boardBackground.setOutlineThickness(2);
    boardBackground.setOutlineColor(Color::Black);
    window.draw(boardBackground);

    // ADD GRID LIne
    VertexArray gridLines(Lines);
    const int lineThickness = 2;
    const Color lineColor = Color(50, 50, 50, 150); // Dark gray rang

    // Vertical lines board ke lia
    for (int x = 0; x <= GridSize; x++) {
        float lineX = startX + x * CandySize;
        gridLines.append(Vertex(Vector2f(lineX, startY), lineColor));
        gridLines.append(Vertex(Vector2f(lineX, startY + GridSize * CandySize), lineColor));
    }

    // Horizontal lines  board ke lia
    for (int y = 0; y <= GridSize; y++) {
        float lineY = startY + y * CandySize;
        gridLines.append(Vertex(Vector2f(startX, lineY), lineColor));
        gridLines.append(Vertex(Vector2f(startX + GridSize * CandySize, lineY), lineColor));
    }

    window.draw(gridLines);

    Sprite candy;
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y < GridSize; y++) {
            int type = GameGrid[x][y];

            if (type >= STRIPED_RED_H && type <= STRIPED_ORANGE_H) {
                candy.setTexture(assets.stripedH[(type - STRIPED_RED_H) % 5]);
            }
            else if (type >= STRIPED_RED_V && type <= STRIPED_ORANGE_V) {
                candy.setTexture(assets.stripedV[(type - STRIPED_RED_V) % 5]);
            }
            else if (type == COLOR_BOMB) {
                candy.setTexture(assets.colorBomb);
            }
            // ADDED: Wrapped Candy makinf  
            else if (type >= WRAPPED_RED && type <= WRAPPED_ORANGE) {
                int wrappedIndex = type - WRAPPED_RED;
                candy.setTexture(assets.wrapped[wrappedIndex]);
            }
            else if (type >= RED && type <= ORANGE) {
                switch (type) {
                case RED: candy.setTexture(assets.red); break;
                case YELLOW: candy.setTexture(assets.yellow); break;
                case GREEN: candy.setTexture(assets.green); break;
                case BLUE: candy.setTexture(assets.blue); break;
                case ORANGE: candy.setTexture(assets.orange); break;
                }
            }

            candy.setPosition(startX + x * CandySize, startY + y * CandySize);
            window.draw(candy);
        }
    }

    if (selection.x != -1 && selection.y != -1) {
        selection.selector.setPosition(
            startX + selection.x * CandySize,
            startY + selection.y * CandySize
        );
        window.draw(selection.selector);
    }
}


int GetPointsForCandy(int type) {
    switch (type) {
    case RED:    return 30;
    case YELLOW: return 30;
    case GREEN:  return 40;
    case BLUE:   return 50;
    case ORANGE: return 60;
    default:     return 0;
    }
}
bool CheckMatches() {
    bool foundMatch = false;
    //  mr.l shape with 4 orientations
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y < GridSize; y++) {
            int base = GameGrid[x][y];
            if (base < RED || base > ORANGE) continue;

            // Horizontal L shape with 3 right + 2 down this is the pehle orientation
            if (x <= GridSize - 3 && y <= GridSize - 3 &&
                GameGrid[x + 1][y] == base &&
                GameGrid[x + 2][y] == base &&
                GameGrid[x + 2][y + 1] == base &&
                GameGrid[x + 2][y + 2] == base)
            {
                GameGrid[x][y] = GameGrid[x + 1][y] = GameGrid[x + 2][y] =
                    GameGrid[x + 2][y + 1] = GameGrid[x + 2][y + 2] = EMPTY;
                GameGrid[x + 2][y] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // Horizontal L-shape with 3 right and 2 up
            if (x <= GridSize - 3 && y >= 2 &&
                GameGrid[x + 1][y] == base &&
                GameGrid[x + 2][y] == base &&
                GameGrid[x + 2][y - 1] == base &&
                GameGrid[x + 2][y - 2] == base)
            {
                GameGrid[x][y] = GameGrid[x + 1][y] = GameGrid[x + 2][y] =
                    GameGrid[x + 2][y - 1] = GameGrid[x + 2][y - 2] = EMPTY;
                GameGrid[x + 2][y] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // Vertical L shape with 3 down and 2 right
            if (x <= GridSize - 3 && y <= GridSize - 3 &&
                GameGrid[x][y + 1] == base &&
                GameGrid[x][y + 2] == base &&
                GameGrid[x + 1][y + 2] == base &&
                GameGrid[x + 2][y + 2] == base)
            {
                GameGrid[x][y] = GameGrid[x][y + 1] = GameGrid[x][y + 2] =
                    GameGrid[x + 1][y + 2] = GameGrid[x + 2][y + 2] = EMPTY;
                GameGrid[x][y + 2] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // Vertical L-shape with 3 down and 2 left
            if (x >= 2 && y <= GridSize - 3 &&
                GameGrid[x][y + 1] == base &&
                GameGrid[x][y + 2] == base &&
                GameGrid[x - 1][y + 2] == base &&
                GameGrid[x - 2][y + 2] == base)
            {
                GameGrid[x][y] = GameGrid[x][y + 1] = GameGrid[x][y + 2] =
                    GameGrid[x - 1][y + 2] = GameGrid[x - 2][y + 2] = EMPTY;
                GameGrid[x][y + 2] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }
        }
    }
    //Mr. T shape :)
    for (int x = 1; x < GridSize - 1; x++) {
        for (int y = 1; y < GridSize - 1; y++) {
            int base = GameGrid[x][y];
            if (base < RED || base > ORANGE) continue;

            // T shape: Horizontal 3 + Vertical Down 2
            if (GameGrid[x - 1][y] == base &&
                GameGrid[x][y] == base &&  // Center 
                GameGrid[x + 1][y] == base &&
                GameGrid[x][y + 1] == base &&
                GameGrid[x][y + 2] == base)
            {
                // Clear T-shape and place wrapped candy
                GameGrid[x - 1][y] = EMPTY;
                GameGrid[x][y] = EMPTY;
                GameGrid[x + 1][y] = EMPTY;
                GameGrid[x][y + 1] = EMPTY;
                GameGrid[x][y + 2] = EMPTY;
                GameGrid[x][y] = WRAPPED_RED + (base - RED); // Center
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // T-shape: Horizontal 3 + Vertical Up 2
            if (GameGrid[x - 1][y] == base &&
                GameGrid[x][y] == base &&  // Center
                GameGrid[x + 1][y] == base &&
                GameGrid[x][y - 1] == base &&
                GameGrid[x][y - 2] == base)
            {
                GameGrid[x - 1][y] = EMPTY;
                GameGrid[x][y] = EMPTY;
                GameGrid[x + 1][y] = EMPTY;
                GameGrid[x][y - 1] = EMPTY;
                GameGrid[x][y - 2] = EMPTY;
                GameGrid[x][y] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // T shape: Vertical 3 + Horizontal Right 2
            if (GameGrid[x][y - 1] == base &&
                GameGrid[x][y] == base &&  // Center
                GameGrid[x][y + 1] == base &&
                GameGrid[x + 1][y] == base &&
                GameGrid[x + 2][y] == base)
            {
                GameGrid[x][y - 1] = EMPTY;
                GameGrid[x][y] = EMPTY;
                GameGrid[x][y + 1] = EMPTY;
                GameGrid[x + 1][y] = EMPTY;
                GameGrid[x + 2][y] = EMPTY;
                GameGrid[x][y] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }

            // T shape: Vertical 3 and Horizontal Left 2
            if (GameGrid[x][y - 1] == base &&
                GameGrid[x][y] == base &&  // Center
                GameGrid[x][y + 1] == base &&
                GameGrid[x - 1][y] == base &&
                GameGrid[x - 2][y] == base)
            {
                GameGrid[x][y - 1] = EMPTY;
                GameGrid[x][y] = EMPTY;
                GameGrid[x][y + 1] = EMPTY;
                GameGrid[x - 1][y] = EMPTY;
                GameGrid[x - 2][y] = EMPTY;
                GameGrid[x][y] = WRAPPED_RED + (base - RED);
                Score += 5 * GetPointsForCandy(base);
                foundMatch = true;
            }
        }
    }


    // Horizontal Color Bomb (5R)
    for (int y = 0; y < GridSize; y++) {
        for (int x = 0; x <= GridSize - 5; x++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                GameGrid[x + 1][y] == baseType &&
                GameGrid[x + 2][y] == baseType &&
                GameGrid[x + 3][y] == baseType &&
                GameGrid[x + 4][y] == baseType) {

                int pointsPerCandy = 0;
                switch (baseType) {
                case RED:    pointsPerCandy = 30; break;
                case YELLOW: pointsPerCandy = 30; break;
                case GREEN:  pointsPerCandy = 40; break;
                case BLUE:   pointsPerCandy = 50; break;
                case ORANGE: pointsPerCandy = 60; break;
                default:     pointsPerCandy = 0;  break;
                }
                Score += 5 * pointsPerCandy;
                GameGrid[x + 2][y] = COLOR_BOMB;
                GameGrid[x][y] = GameGrid[x + 1][y] = GameGrid[x + 3][y] = GameGrid[x + 4][y] = EMPTY;
                foundMatch = true;
            }
        }
    }

    // Vertical Color Bomb (5C)
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y <= GridSize - 5; y++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                GameGrid[x][y + 1] == baseType &&
                GameGrid[x][y + 2] == baseType &&
                GameGrid[x][y + 3] == baseType &&
                GameGrid[x][y + 4] == baseType) {

                int pointsPerCandy = 0;
                switch (baseType) {
                case RED:    pointsPerCandy = 30; break;
                case YELLOW: pointsPerCandy = 30; break;
                case GREEN:  pointsPerCandy = 40; break;
                case BLUE:   pointsPerCandy = 50; break;
                case ORANGE: pointsPerCandy = 60; break;
                default:     pointsPerCandy = 0;  break;
                }
                Score += 5 * pointsPerCandy;
                GameGrid[x][y + 2] = COLOR_BOMB;
                GameGrid[x][y] = GameGrid[x][y + 1] = GameGrid[x][y + 3] = GameGrid[x][y + 4] = EMPTY;
                foundMatch = true;
            }
        }
    }

    // Horizontal 4 (Striped candy)
    for (int y = 0; y < GridSize; y++) {
        for (int x = 0; x <= GridSize - 4; x++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                GameGrid[x + 1][y] == baseType &&
                GameGrid[x + 2][y] == baseType &&
                GameGrid[x + 3][y] == baseType) {

                int striped = STRIPED_RED_H + (baseType - RED);
                GameGrid[x][y] = GameGrid[x + 1][y] = GameGrid[x + 2][y] = GameGrid[x + 3][y] = EMPTY;
                GameGrid[x + 2][y] = striped;
                Score += 4 * GetPointsForCandy(baseType);
                foundMatch = true;
            }
        }
    }

    // Vertical 4 (Striped candy)
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y <= GridSize - 4; y++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                GameGrid[x][y + 1] == baseType &&
                GameGrid[x][y + 2] == baseType &&
                GameGrid[x][y + 3] == baseType) {

                int striped = STRIPED_RED_V + (baseType - RED);
                GameGrid[x][y] = GameGrid[x][y + 1] = GameGrid[x][y + 2] = GameGrid[x][y + 3] = EMPTY;
                GameGrid[x][y + 2] = striped;
                Score += 4 * GetPointsForCandy(baseType);
                foundMatch = true;
            }
        }
    }

    // Horizontal 3 
    for (int y = 0; y < GridSize; y++) {
        for (int x = 0; x < GridSize - 2; x++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                baseType == GameGrid[x + 1][y] &&
                baseType == GameGrid[x + 2][y]) {

                bool hasSpecial = false;
                for (int i = 0; i < 3; i++) {
                    int type = GameGrid[x + i][y];
                    // Rule 5: Striped Candy Handling
                    if (type >= STRIPED_RED_H && type <= STRIPED_ORANGE_V) {
                        hasSpecial = true;
                        if (type <= STRIPED_ORANGE_H) { // Horizontal striped
                            for (int col = 0; col < GridSize; col++) {
                                if (GameGrid[col][y] != EMPTY) Score += 10;
                                GameGrid[col][y] = EMPTY;
                            }
                        }
                        else { // Vertical striped
                            int col = x + i;
                            for (int row = 0; row < GridSize; row++) {
                                if (GameGrid[col][row] != EMPTY) Score += 10;
                                GameGrid[col][row] = EMPTY;
                            }
                        }
                        break;
                    }
                    // Rule 6: Wrapped Candy Handling (3x3 dhamaka)
                    else if (type >= WRAPPED_RED && type <= WRAPPED_ORANGE) {
                        hasSpecial = true;
                        for (int dx = -1; dx <= 1; dx++) {
                            for (int dy = -1; dy <= 1; dy++) {
                                int nx = x + i + dx;
                                int ny = y + dy;
                                if (nx >= 0 && nx < GridSize && ny >= 0 && ny < GridSize) {
                                    if (GameGrid[nx][ny] != EMPTY) Score += 10;
                                    GameGrid[nx][ny] = EMPTY;
                                }
                            }
                        }
                        break;
                    }
                }

                if (!hasSpecial) {
                    GameGrid[x][y] = GameGrid[x + 1][y] = GameGrid[x + 2][y] = EMPTY;
                    Score += 3 * GetPointsForCandy(baseType);
                }
                foundMatch = true;
            }
        }
    }


    // Vertical 3
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y < GridSize - 2; y++) {
            int baseType = GameGrid[x][y];
            if (baseType != EMPTY &&
                baseType == GameGrid[x][y + 1] &&
                baseType == GameGrid[x][y + 2]) {

                bool hasSpecial = false;
                for (int i = 0; i < 3; i++) {
                    int type = GameGrid[x][y + i];
                    // Rule 5: Striped Candy Handling
                    if (type >= STRIPED_RED_H && type <= STRIPED_ORANGE_V) {
                        hasSpecial = true;
                        if (type <= STRIPED_ORANGE_H) { // Horizontal striped
                            for (int col = 0; col < GridSize; col++) {
                                if (GameGrid[col][y + i] != EMPTY) Score += 10;
                                GameGrid[col][y + i] = EMPTY;
                            }
                        }
                        else { // Vertical striped
                            int row = y + i;
                            for (int col = 0; col < GridSize; col++) {
                                if (GameGrid[col][row] != EMPTY) Score += 10;
                                GameGrid[col][row] = EMPTY;
                            }
                        }
                        break;
                    }
                    // Rule 6: Wrapped Candy Handling (3x3 dhamaka)
                    else if (type >= WRAPPED_RED && type <= WRAPPED_ORANGE) {
                        hasSpecial = true;
                        for (int dx = -1; dx <= 1; dx++) {
                            for (int dy = -1; dy <= 1; dy++) {
                                int nx = x + dx;
                                int ny = y + i + dy;
                                if (nx >= 0 && nx < GridSize && ny >= 0 && ny < GridSize) {
                                    if (GameGrid[nx][ny] != EMPTY) Score += 10;
                                    GameGrid[nx][ny] = EMPTY;
                                }
                            }
                        }
                        break;
                    }
                }

                if (!hasSpecial) {
                    GameGrid[x][y] = GameGrid[x][y + 1] = GameGrid[x][y + 2] = EMPTY;
                    Score += 3 * GetPointsForCandy(baseType);
                }
                foundMatch = true;
            }
        }
    }



    return foundMatch;
}
void ShiftCandiesDown() {
    for (int x = 0; x < GridSize; x++) {
        for (int y = GridSize - 1; y >= 0; y--) {
            if (GameGrid[x][y] == EMPTY) {
                for (int ny = y - 1; ny >= 0; ny--) {
                    if (GameGrid[x][ny] != EMPTY) {
                        GameGrid[x][y] = GameGrid[x][ny];
                        GameGrid[x][ny] = EMPTY;
                        break;
                    }
                }
            }
        }
    }

    // Fill empty spots with new candies
    for (int x = 0; x < GridSize; x++) {
        for (int y = 0; y < GridSize; y++) {
            if (GameGrid[x][y] == EMPTY) {
                GameGrid[x][y] = rand() % 5 + 1;
            }
        }
    }

}

// Function Implementations yaane grids banaata
void InitializeGrid() {
    srand(time(0));
    bool hasInitialMatches = true;

    // Keep regenerating the grid until no starting matches exist
    while (hasInitialMatches) {
        // Fill grid with random candies
        for (int i = 0; i < GridSize; i++) {
            for (int j = 0; j < GridSize; j++) {
                GameGrid[i][j] = rand() % 5 + 1;
            }
        }

        // Check for matches and shift until grid is stable
        hasInitialMatches = false;
        while (CheckMatches()) {
            ShiftCandiesDown();
            hasInitialMatches = true;
        }
    }

    // Reset score and moves when starting a new game
    Score = 0;
    Moves = 20;
}



bool needsSpecialEffect = false;
int specialEffectType = 0;  
int effectX = -1, effectY = -1;
int effectColor = EMPTY;     

void HandleGameInput(Event& event, RenderWindow& window) {
    if (event.type == Event::MouseButtonPressed) {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        // Check save button click during gameplay
        if (currentState == PLAYING) {
            FloatRect saveButton(WindowSize.x - 140, 10, 120, 40);
            if (saveButton.contains(mousePos)) {
                SaveGame();
                return;
            }
            FloatRect exitButton(WindowSize.x - 270, 10, 120, 40);
            if (exitButton.contains(mousePos)) {
                window.close();
                return;
            }
        }
        float startX = (WindowSize.x - GridSize * CandySize) / 2;
        float startY = (WindowSize.y - GridSize * CandySize) / 2;


        int gridX = (int)((mousePos.x - startX) / CandySize);
        int gridY = (int)((mousePos.y - startY) / CandySize);
        gridX = gridX < 0 ? 0 : (gridX >= GridSize ? GridSize - 1 : gridX);
        gridY = gridY < 0 ? 0 : (gridY >= GridSize ? GridSize - 1 : gridY);

        if (gridX >= 0 && gridX < GridSize && gridY >= 0 && gridY < GridSize) {
            if (selection.x == -1) {
                selection.x = gridX;
                selection.y = gridY;
            }
            else {
                if (selection.x < 0 || selection.x >= GridSize ||
                    selection.y < 0 || selection.y >= GridSize) {
                    selection.x = -1;
                    selection.y = -1;
                    return;
                }

                if ((abs(selection.x - gridX) == 1 && selection.y == gridY) ||
                    (abs(selection.y - gridY) == 1 && selection.x == gridX)) {

                    if (selection.x >= 0 && selection.x < GridSize &&
                        selection.y >= 0 && selection.y < GridSize &&
                        gridX >= 0 && gridX < GridSize &&
                        gridY >= 0 && gridY < GridSize) {

                        // swap
                        int temp = GameGrid[selection.x][selection.y];
                        GameGrid[selection.x][selection.y] = GameGrid[gridX][gridY];
                        GameGrid[gridX][gridY] = temp;
                        Moves--;

                        int type1 = GameGrid[selection.x][selection.y];
                        int type2 = GameGrid[gridX][gridY];


                        needsSpecialEffect = false;
                        effectX = gridX;
                        effectY = gridY;

                        if (type1 == COLOR_BOMB || type2 == COLOR_BOMB) {
                            needsSpecialEffect = true;
                            if (type1 == COLOR_BOMB && type2 == COLOR_BOMB) {
                                specialEffectType = 8;
                            }
                            else {
                                int otherType = (type1 == COLOR_BOMB) ? type2 : type1;
                                if (otherType >= RED && otherType <= ORANGE) {
                                    specialEffectType = 7;
                                    effectColor = otherType;
                                }
                                else if (otherType >= STRIPED_RED_H && otherType <= STRIPED_ORANGE_V) {
                                    specialEffectType = 9;
                                    effectColor = (otherType - STRIPED_RED_H) % 5 + RED;
                                }
                                else if (otherType >= WRAPPED_RED && otherType <= WRAPPED_ORANGE) {
                                    specialEffectType = 10;
                                    effectColor = otherType - WRAPPED_RED + RED;
                                }
                            }
                        }
                        else if ((type1 >= STRIPED_RED_H && type2 >= WRAPPED_RED) ||
                            (type2 >= STRIPED_RED_H && type1 >= WRAPPED_RED)) {
                            needsSpecialEffect = true;
                            specialEffectType = 11;  
                        }
                        else if (type1 >= WRAPPED_RED && type2 >= WRAPPED_RED) {
                            needsSpecialEffect = true;
                            specialEffectType = 12; 
                        }
                    }

                    selection.x = -1;
                    selection.y = -1;
                }
            }
        }
    }
}


void HandleMenuInput(Event& event, RenderWindow& window) {
    if (event.type == Event::MouseButtonPressed) {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        FloatRect playButton(WindowSize.x / 2 - 100, WindowSize.y / 2 + 115, 200, 60);
        FloatRect saveButton(WindowSize.x / 2 - 100, WindowSize.y / 2 - 20, 200, 60);
        FloatRect loadButton(WindowSize.x / 2 - 100, WindowSize.y / 2 + 180, 200, 60);
        if (playButton.contains(mousePos)) {
            currentState = PLAYING;
            InitializeGrid();
            Score = 0;
            Moves = 20;
        }
        else if (saveButton.contains(mousePos)) {
            SaveGame();
        }
        else if (loadButton.contains(mousePos)) {
            LoadGame();
            currentState = PLAYING;
        }

        if (currentState == GAME_OVER || currentState == WINNER) {
            FloatRect retryButton(WindowSize.x / 2 - 100, WindowSize.y / 2 + 30, 200, 60);
            if (retryButton.contains(mousePos)) {
                currentState = PLAYING;
                InitializeGrid();
                Score = 0;
                Moves = 20;
                return;
            }
        }
    }
}

void DrawUI(RenderWindow& window, GameAssets& assets) {
    if (currentState != PLAYING) return;

    Text scoreText("Score: " + to_string(Score), assets.font, 24);
    Text movesText("Moves: " + to_string(Moves), assets.font, 24);
    Text targetText("Target: " + to_string(TargetScore), assets.font, 24);
    RectangleShape exitButton(Vector2f(120, 40));
    exitButton.setPosition(WindowSize.x - 270, 10);
    exitButton.setFillColor(Color(200, 200, 200));

    Text exitText("Exit", assets.font, 20);
    exitText.setFillColor(Color::Black);
    exitText.setPosition(WindowSize.x - 265, 15);


    scoreText.setFillColor(Color::Black);
    movesText.setFillColor(Color::Black);
    targetText.setFillColor(Color::Black);

    scoreText.setPosition(20, 20);
    movesText.setPosition(20, 50);
    targetText.setPosition(20, 80);

    window.draw(scoreText);
    window.draw(movesText);
    window.draw(targetText);
    if (currentState == PLAYING) {
        RectangleShape saveButton(Vector2f(120, 40));
        saveButton.setPosition(WindowSize.x - 140, 10);
        saveButton.setFillColor(Color(200, 200, 200));

        Text saveText("Save Game", assets.font, 20);
        saveText.setFillColor(Color::Black);
        saveText.setPosition(WindowSize.x - 135, 15);

        window.draw(saveButton);
        window.draw(saveText);
        window.draw(exitButton);
        window.draw(exitText);
    }

}


void HandleSpecialEffects() {
    if (!needsSpecialEffect) return;

    switch (specialEffectType) {
    case 7: // Color Bomb + Plain Candy
    {
        GameGrid[effectX][effectY] = EMPTY; // Clear the Color Bomb itself
        int crushed = 0;
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                if (GameGrid[x][y] == effectColor) {
                    GameGrid[x][y] = EMPTY;
                    crushed++;
                }
            }
        }
        Score += crushed * GetPointsForCandy(effectColor); 
        break;
    }


    case 8: // Color Bomb + Color Bomb
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                if (GameGrid[x][y] != EMPTY) {
                    int type = GameGrid[x][y];
                    Score += GetPointsForCandy(type);
                }
                GameGrid[x][y] = EMPTY;
            }
        }
        break;

    case 9: // Color Bomb + Striped
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                if (GameGrid[x][y] == effectColor) {
                    GameGrid[x][y] = STRIPED_RED_H + (effectColor - RED);
                }
            }
        }
        break;

    case 10: // Color Bomb + Wrapped
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                if (GameGrid[x][y] == effectColor) {
                    GameGrid[x][y] = WRAPPED_RED + (effectColor - RED);
                }
            }
        }
        // khatam 3x5 area around  position
        for (int dx = -1; dx <= 1; dx++) {  // 3r
            for (int dy = -2; dy <= 2; dy++) {//5c
                int x = effectX + dx;
                int y = effectY + dy;
                if (x >= 0 && x < GridSize && y >= 0 && y < GridSize) {
                    if (GameGrid[x][y] != EMPTY) {
                        int type = GameGrid[x][y];
                        Score += GetPointsForCandy(type);
                    }
                    GameGrid[x][y] = EMPTY;
                }
            }
        }

        break;


    case 11: { // Striped + Wrapped Candy
        // Clear the entire row/column (striped effect)
        if (effectColor >= STRIPED_RED_H && effectColor <= STRIPED_ORANGE_H) { // Horizontal striped
            for (int col = 0; col < GridSize; col++) {
                if (GameGrid[col][effectY] != EMPTY) {
                    Score += GetPointsForCandy(GameGrid[col][effectY]);
                }
                GameGrid[col][effectY] = EMPTY;
            }
        }
        else if (effectColor >= STRIPED_RED_V && effectColor <= STRIPED_ORANGE_V) { // Vertical striped
            for (int row = 0; row < GridSize; row++) {
                if (GameGrid[effectX][row] != EMPTY) {
                    int type = GameGrid[effectX][row];
                    Score += GetPointsForCandy(type);
                }

                GameGrid[effectX][row] = EMPTY;
            }
        }

        // 3x3 explosion (wrapped effect)
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int x = effectX + dx;
                int y = effectY + dy;
                if (x >= 0 && x < GridSize && y >= 0 && y < GridSize) {
                    if (GameGrid[x][y] != EMPTY) {
                        int type = GameGrid[x][y];
                        Score += GetPointsForCandy(type);
                    }
                    GameGrid[x][y] = EMPTY;
                }
            }
        }
        break;
    }

    case 12: { // Wrapped + Wrapped
        for (int dx = -2; dx <= 2; dx++) { // 5 rows
            for (int dy = -2; dy <= 2; dy++) { // 5 columns
                int x = effectX + dx;
                int y = effectY + dy;
                if (x >= 0 && x < GridSize && y >= 0 && y < GridSize) {
                    if (GameGrid[x][y] != EMPTY) {
                        Score += GetPointsForCandy(GameGrid[x][y]);
                    }
                    GameGrid[x][y] = EMPTY;
                }
            }
        }
        break;
    }
    }

    needsSpecialEffect = false;
    ShiftCandiesDown();
}
// Function Implementations yaane grids banaata

int main() {
    RenderWindow window(VideoMode(WindowSize.x, WindowSize.y), "Candy Crush");
    GameAssets assets;
    Clock gameClock;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            switch (currentState) {
            case MENU:
            case GAME_OVER:
                HandleMenuInput(event, window);
                break;
            case WINNER:
                HandleMenuInput(event, window);
                break;
            case PLAYING:
                HandleGameInput(event, window);
                break;
            default: break;
            }
        }


        if (currentState == PLAYING) {
            HandleSpecialEffects();  

            if (gameClock.getElapsedTime().asSeconds() > 0.5f) {
                bool hadMatches = CheckMatches();
                if (hadMatches) {
                    ShiftCandiesDown();
                }
                gameClock.restart();
            }

            if (Score >= TargetScore) {
                currentState = WINNER;
            }
            else if (Moves <= 0) {
                currentState = GAME_OVER;
            }
        }

        window.clear();
        DrawGrid(window, assets);
        DrawUI(window, assets);
        window.display();
    }
    return 0;
}