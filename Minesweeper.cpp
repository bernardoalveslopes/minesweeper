#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>
#include <cell.cpp>

using namespace std;

void AssortBoard(vector<vector<Cell>>& board, int mineCount, int width, int height);

void loadBoardFromFile(const string& filename, vector<vector<Cell>>& board, float tileSize, int& mineCount, int& totalSafeCells);

int main() {

    ifstream config("boards/config.cfg");
    int width, height, mineCount;

    if (config.is_open()) {
        config >> width >> height >> mineCount;
        config.close();
    }

    else {
        cout << "config file didn't open L" << endl;
    }

    const int tileSize = 32;
    int windowWidth = width * tileSize;
    // pdf asked for height + 100 for the status bar and smileys
    int windowHeight = height * tileSize + 100;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Minesweeper");

    // creating the smiley and buttons
    sf::Texture happyFaceTexture;
    if (!happyFaceTexture.loadFromFile("images/face_happy.png")) {
        cout << "Failed to load face_happy" << endl;
    }

    sf::Texture sadFaceTexture;
    if (!sadFaceTexture.loadFromFile("images/face_lose.png")) {
        cout << "Failed to load face_lose" << endl;
    }

    sf::Texture coolFaceTexture;
    if (!coolFaceTexture.loadFromFile("images/face_win.png")) {
        cout << "Failed to load face_win" << endl;
    }

    sf::Texture debugTexture;
    if (!debugTexture.loadFromFile("images/debug.png")) {
        cout << "Failed to load debug" << endl;
    }

    sf::Texture test1Texture;
    if (!test1Texture.loadFromFile("images/test_1.png")) {
        cout << "Failed to load test1" << endl;
    }

    sf::Texture test2Texture;
    if (!test2Texture.loadFromFile("images/test_2.png")) {
        cout << "Failed to load test2" << endl;
    }

    sf::Texture test3Texture;
    if (!test3Texture.loadFromFile("images/test_3.png")) {
        cout << "Failed to load test3" << endl;
    }

    sf::Texture digitsTexture;
    if (!digitsTexture.loadFromFile("images/digits.png")) {
        cout << "Failed to load digits.png" << endl;
    }

    sf::Sprite happyFaceSprite(happyFaceTexture);
    // thomas said on slack that the positions dont have to be exact, so im just putting it in the middle (divided by 2)
    happyFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite sadFaceSprite(sadFaceTexture);
    sadFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite coolFaceSprite(coolFaceTexture);
    coolFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite debugSprite(debugTexture);
    // behold, the part where I try different x and y coordinates 42159 times until it looks nice
    debugSprite.setPosition((windowWidth - 320), height * tileSize + (100 - 64) / 2);

    sf::Sprite test1Sprite(test1Texture);
    test1Sprite.setPosition((windowWidth - 250), height * tileSize + (100 - 64) / 2);

    sf::Sprite test2Sprite(test2Texture);
    test2Sprite.setPosition((windowWidth - 180), height * tileSize + (100 - 64) / 2);

    sf::Sprite test3Sprite(test3Texture);
    test3Sprite.setPosition((windowWidth - 110), height * tileSize + (100 - 64) / 2);

    vector<vector<Cell>> board;
    for (int y = 0; y < height; ++y) {
        vector<Cell> row;
        for (int x = 0; x < width; ++x) {
            row.emplace_back(x * tileSize, y * tileSize, tileSize);
        }
        board.push_back(row);
    }

    // assort the board

    AssortBoard(board, mineCount, width, height);

    bool gameWin = false;
    bool gameEnd = false;
    bool gameLose = false;
    static bool debugEnabled = false;

    int totalSafeCells = (width * height) - mineCount;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int cursorX = static_cast<int>(mousePos.x / tileSize);
            int cursorY = static_cast<int>(mousePos.y / tileSize);

            // same click inputs as before, just check if the game is running. The only clicks that will register are the menus so they can be out the if statement



            if (!gameEnd) {


                // trying to check if the stuff is properly being laid out
                if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                    const CellState& cellState = board[cursorY][cursorX].getState();
                    // cout << "Hovering over (" << cursorX << ", " << cursorY << ")" << endl;
                    // if (cellState.unrevealed) cout << "Unrevealed ";
                    // if (cellState.revealed) cout << "Revealed ";
                    // if (cellState.mine) cout << "Mine ";
                    // if (cellState.flag) cout << "Flag ";
                    // if (cellState.number) cout << "Number ";
                    // these have been SO USEFUL BECAUSE THE MINES KEEP NOT WORKING EVEN THOUGH I AM CLICKING ON THEM AGGHHH
                }

                // click time
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                            // we can just always call reveal neighbors, since it has reveal within it
                            board[cursorY][cursorX].revealNeighbors(board, cursorX, cursorY, width, height);
                            const CellState& cellState = board[cursorY][cursorX].getState();
                            if (cellState.mine) {
                                gameLose = true;
                                gameEnd = true;
                                // need to get state of every tile, then call reveal if it is a mine
                                for (int y = 0; y < height; y++) {
                                    for (int x = 0; x < width; x++) {
                                        const CellState& currentState = board[y][x].getState();
                                        if (currentState.mine) {
                                            board[y][x].reveal();
                                        }
                                    }
                                }
                            }

                        }
                    }

                    // these F'ing flags

                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                            const CellState& cellState = board[cursorY][cursorX].getState();
                            if (cellState.unrevealed) {
                                if (!cellState.flag) {
                                    board[cursorY][cursorX].placeFlag();
                                    mineCount = mineCount - 1;
                                }
                                else {
                                    board[cursorY][cursorX].removeFlag();
                                    mineCount++;

                                }
                            }

                        }
                    }
                }

            



                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::FloatRect debugBounds = debugSprite.getGlobalBounds();

                    if (debugBounds.contains(mousePos)) {
                        debugEnabled = !debugEnabled;

                       //  cout << "debug mode enabled" << endl;

                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                const CellState& currentState = board[y][x].getState();
                                if (currentState.mine) {
                                    if (debugEnabled) {
                                        board[y][x].reveal();
                                    }
                                    else {
                                        if (currentState.flag) {
                                            board[y][x].setState({ true, false, true, true, false });
                                        }
                                        else {
                                            board[y][x].setState({ true, false, true, false, false });
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }

            // this checks if the smiley was clicked. This can be outside the if statement above, since it can be clicked at any time during the game
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect happyFaceBounds = happyFaceSprite.getGlobalBounds();

                if (happyFaceBounds.contains(mousePos)) {
                
                    for (int x = 0; x < width; x++) {
                        for (int y = 0; y < height; y++) {
                            board[y][x].setSurroundingMines(0);
                            board[y][x].setState({ true, false, false, false, false });
                        }
                    }


                    Cell::revealedCells = 0;
    
                    ifstream config("boards/config.cfg");
                    if (config.is_open()) {
                        config >> width >> height >> mineCount;
                        config.close();
                    }
                    AssortBoard(board, mineCount, width, height);
                    // fixed :D
                    gameLose = false;
                    gameEnd = false;
                    gameWin = false;
                    debugEnabled = false;
                    totalSafeCells = (width * height) - mineCount;

                }
            }




            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test1Bounds = test1Sprite.getGlobalBounds();

                if (test1Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;
                    loadBoardFromFile("boards/testboard1.brd", board, tileSize, mineCount, totalSafeCells);


                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test2Bounds = test2Sprite.getGlobalBounds();

                if (test2Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;
                    loadBoardFromFile("boards/testboard2.brd", board, tileSize, mineCount, totalSafeCells);


                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test3Bounds = test3Sprite.getGlobalBounds();

                if (test3Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;

                    loadBoardFromFile("boards/testboard3.brd", board, tileSize, mineCount, totalSafeCells);

                }
            }




            if (totalSafeCells == Cell::revealedCells) {
                gameWin = true;
                gameEnd = true;
                // place a flag on unrevealed tiles once won
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        const CellState& currentState = board[y][x].getState();
                        if (currentState.mine) {
                            // placeflag only works with unrevealed cells, so this just sets all of them to not have mines (which should happen if you won anyway)
                            // this also helps with making the debug button not do anything after winning
                            board[y][x].setState({ true, false, false, false, false });
                            board[y][x].placeFlag();
                        }
                    }
                }
            }
        }


        window.clear();

        for (auto& row : board) {
            for (auto& cell : row) {
                cell.draw(window);
            }
        }

        window.draw(test1Sprite);
        window.draw(test2Sprite);
        window.draw(test3Sprite);
        if (!gameEnd) {
            window.draw(happyFaceSprite);
        }

        else if (gameEnd && gameLose) {
            window.draw(sadFaceSprite);
        }
        else if (gameEnd && gameWin) {
            window.draw(coolFaceSprite);
        }
        window.draw(debugSprite);
        drawNumber(window, digitsTexture, mineCount, 30, height * tileSize + 10);

        window.display();
    }

    Cell::cleanUp();
    return 0;
}

void AssortBoard(vector<vector<Cell>>& board, int mineCount, int width, int height) {

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < mineCount; ++i) {
        int randX = rand() % width;
        int randY = rand() % height;
        if (!board[randY][randX].isMine()) {
            // THIS ONE LINE CAUSED ME HOURS UPON HOURS OF STRIFE (i accidentally set first to false)
            board[randY][randX].setState({ true, false, true, false, false });
        }
        else {
            --i;
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (board[y][x].isMine()) continue;

            int mineCount = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int k = x + j;
                    int l = y + i;
                    if (k >= 0 && k < width && l >= 0 && l < height) {
                        if (board[l][k].isMine()) {
                            ++mineCount;
                        }
                    }
                }
            }
            if (mineCount > 0) {
                board[y][x].setSurroundingMines(mineCount);
            }
        }
    }
}


void loadBoardFromFile(const string& filename, vector<vector<Cell>>& board, float tileSize, int& mineCount, int& totalSafeCells) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }

    const int rows = 16;
    const int cols = 25;

    board.clear();
    board.resize(rows, vector<Cell>(cols, Cell(0, 0, tileSize)));

    mineCount = 0;
    totalSafeCells = 400;

    string line;
    int row = 0;

    while (getline(file, line) && row < rows) {
        for (int col = 0; col < cols && col < line.length(); ++col) {
            float x = col * tileSize;
            float y = row * tileSize;

            Cell& cell = board[row][col];
            cell = Cell(x, y, tileSize);


            if (line[col] == '1') {
                CellState state;
                state.unrevealed = true;
                state.mine = true;
                cell.setState(state);
                mineCount++;
                totalSafeCells--;
            }
            else if (line[col] == '0') {
                CellState state;
                state.unrevealed = true;
                state.mine = false;
                cell.setState(state);
            }
        }
        ++row;
    }

    file.close();


    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int surroundingMines = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !(dr == 0 && dc == 0)) {
                        if (board[nr][nc].isMine()) {
                            ++surroundingMines;
                        }
                    }
                }
            }
            board[r][c].setSurroundingMines(surroundingMines);
        }
    }
}

