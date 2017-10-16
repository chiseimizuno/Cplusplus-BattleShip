#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:
    int m_rows;
    int m_cols;
    int m_nShips = 0;
    int m_shipLength[100];
    int m_shipSymbol[100];
    string m_shipName[100];
    
    //Board m_board;
    
};

void waitForEnter()
{
    //Waits for user to press enter
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
}

//Returns row
int GameImpl::rows() const
{
    return m_rows;
}

//Returns column
int GameImpl::cols() const
{
    return m_cols;
}

//Returns true if point is within the grid
bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

//Retuns a random point in the grid (point will always be valid)
Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

//Sets private variables at the proper index
bool GameImpl::addShip(int length, char symbol, string name)
{
    m_shipLength[m_nShips] = length;
    m_shipSymbol[m_nShips] = symbol;
    m_shipName[m_nShips] = name;
    m_nShips++;
    return true;
}

//Simply returns number of hsips
int GameImpl::nShips() const
{
    return m_nShips;;
}

//Returns length of ship
int GameImpl::shipLength(int shipId) const
{
    return m_shipLength[shipId];
}

//Returns symbol of ship
char GameImpl::shipSymbol(int shipId) const
{
    return m_shipSymbol[shipId];
}

//Returns name of ship
string GameImpl::shipName(int shipId) const
{
    return m_shipName[shipId];
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    //Places ship for player 1 and 2, if ships cannot be placed, game ends by returning nullptr
    if (!p1->placeShips(b1))
        return nullptr;
    if (!p2->placeShips(b2))
        return nullptr;
    
    //Initialization
    bool validShot, shotHit, shipDestroyed;
    int shipId = -1 ;
    
    while (1) //While winner is not selected
    {
        // ************************ Player 1's turn ************************
        //Print & Display Board of P2 for P1
        cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
        b2.display(p1->isHuman());
        //Reset parameters
        validShot = false;
        shotHit = false;
        shipDestroyed = false;
        shipId = -1;
        //Attack and Record results
        Point p1Move = p1->recommendAttack();
        validShot = b2.attack(p1Move, shotHit, shipDestroyed, shipId);
        p1->recordAttackResult(p1Move, validShot, shotHit, shipDestroyed, shipId);
        //Outputs to user the result of the attack and the resulting board
        if (!validShot)
        {
            //If show was valid
            cout << p1->name() << " wasted a shot at (" << p1Move.r << "," << p1Move.c << ")." << endl;
        }
        else
        {
            cout << p1->name() << " attacked (" << p1Move.r << "," << p1Move.c << ") and ";

            if (shotHit == false) //If missed
            {
                cout << "missed, resulting in: " << endl;
            }
            else if (shipDestroyed == true) //If ship was destroyed
            {
                cout << "destroyed the " << shipName(shipId) << ", resulting in:" << endl;
            }
            else if (shotHit == true) //If hit ship but ship not destroyed
            {
                cout << "hit something, resulting in: " << endl;
            }
            b2.display(p1->isHuman());
        }
        
        //Check if player 1 won by destroying all ships
        if (b2.allShipsDestroyed())
        {
            //If the losing player is human, display the winner's board, showing everything
            cout << p1->name() << " wins!" << endl;
            
            if (p2->isHuman())
            {
                cout << "Here's where " << p1->name() << "'s ships were: " << endl;
                b1.display(false);
            }
            return p1;
        }
        //Pause Game
        if (shouldPause)
            waitForEnter();
        
            
        // ************************ Player 2's turn ************************
        
        //Print & Display Board of P1 for P2
        cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
        b1.display(p2->isHuman());
        //Reset parameters
        validShot = false;
        shotHit = false;
        shipDestroyed = false;
        shipId = -1;
        //Attack and record results
        Point p2Move = p2->recommendAttack();
        validShot = b1.attack(p2Move, shotHit, shipDestroyed, shipId);
        p2->recordAttackResult(p2Move, validShot, shotHit, shipDestroyed, shipId);
        //Output to user the result of the attack and the resulting board
        if (!validShot)
        {
            cout << p2->name() << " wasted a shot at (" << p2Move.r << "," << p2Move.c << ")." << endl;
        }
        else
        {
            cout << p2->name() << " attacked (" << p2Move.r << "," << p2Move.c << ") and ";
            if (shotHit == false) //If shot missed
            {
                cout << "missed, resulting in: " << endl;
            }
            else if (shipDestroyed == true) //If ship destroyed
            {
                cout << "destroyed the " << shipName(shipId) << ", resulting in:" << endl;
            }
            else if (shotHit == true) //If hit part of ship but ship not destroyed
            {
                cout << "hit something, resulting in: " << endl;
            }
            b1.display(p2->isHuman());
        }
        
        //If player 2 won by destroying all of p1's ships
        if (b1.allShipsDestroyed())
        {
             //If the losing player is human, display the winner's board, showing everything
            cout << p2->name() << " wins!" << endl;
            if (p1->isHuman())
            {
                cout << "Here's where " << p2->name() << "'s ships were: " << endl;
                b2.display(false);
            }
            return p2;
        }
        
        //Pause game
        if (shouldPause)
            waitForEnter();
    }

    
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

