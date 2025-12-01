#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct CellState { 

    bool unrevealed = true;
    bool revealed = false;
    bool mine = false;
    bool flag = false;
    bool number = false;
};

class Cell {
private:
    sf::Sprite sprite;
    static sf::Texture* hiddenTexture;
    static sf::Texture* revealedTexture;
    static sf::Texture* flagTexture;
    static sf::Texture* numberTextures[8]; // for number 1-8
    static sf::Texture* mineTexture;

    CellState state; // will track the state of the cell 
    int surroundingMines;
    

public:
    // Constructor
    Cell(float x, float y, float tileSize);

    static int revealedCells; 

    void draw(sf::RenderWindow& window);

    void reveal();

    void revealNeighbors(vector<vector<Cell>>& board, int x, int y, int width, int height);

    void placeFlag();

    void removeFlag();

    void setSurroundingMines(int count);

    void setState(const CellState& newState);

    static void cleanUp();

    static void loadTextures();

    const CellState& getState() const;

    int getSurroundingMines() const;

    bool isMine() const;
};

sf::Texture* Cell::hiddenTexture = nullptr;
sf::Texture* Cell::revealedTexture = nullptr;
sf::Texture* Cell::flagTexture = nullptr;
sf::Texture* Cell::numberTextures[8] = { nullptr };
sf::Texture* Cell::mineTexture = nullptr;


