#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
    const Game& m_game;
    char m_grid[MAXROWS][MAXCOLS];
    int m_block[MAXROWS][MAXCOLS];
    bool shipsPlaced[100];
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    // Set the grid to be '.' and m_block to 0. # ships placed should also be all set to false.  
    for (int r = 0; r < 10; r ++)
    {
        for (int c = 0; c < 10; c++)
        {
            m_grid[r][c] = '.';
            m_block[r][c] = 0;
            shipsPlaced[r*10+c] = false;
        }
    }
}


void BoardImpl::clear()
{
    //Clears board, essentially makes everything back to original
    for (int r = 0; r < 10; r ++)
    {
        for (int c = 0; c < 10; c++)
        {
            m_grid[r][c] = '.';
            m_block[r][c] = 0;
            shipsPlaced[r*10+c] = false;
        }
    }
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                m_block[r][c] = 1; //Block contains 1 if that area is to be blocked
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            m_block[r][c] = 0; //Turns block grid to all 0s
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    // Return false if ShipId negative or more than stored
    if (shipId < 0 || shipId >= m_game.nShips())
        return false;
    // Return false if exceeds vertical boundary
    if (dir == VERTICAL && topOrLeft.r < 0)
        return false;
    if (dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
        return false;
    //Return false if exceeds horinzontal boundary
    if (dir == HORIZONTAL && topOrLeft.c < 0)
        return false;
    if (dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
        return false;
    // if ship is vertical and trying to be placed on blocked location or another ship
    if (dir == VERTICAL)
        for (int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId) ; r++)
            if (m_block[r][topOrLeft.c] == 1 || m_grid[r][topOrLeft.c] != '.')
                return false;
    // if ship is horizontal and trying to be placed on blocked location or another ship
    if (dir == HORIZONTAL)
        for (int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId) ; c++)
            if (m_block[topOrLeft.r][c] == 1 || m_grid[topOrLeft.r][c] != '.')
                return false;
                
    //If ship has already been placed on the board
    if (shipsPlaced[shipId])
        return false;
    
    //Ship placed, change board grid to reflect new ship, and return true
    shipsPlaced[shipId] = true;
    
    //Adds the ship symbol at the right locations on the board, for both Vertical and Horizontal cases
    if (dir == VERTICAL) //Vertical
        for (int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId) ; r++)
            m_grid[r][topOrLeft.c] = m_game.shipSymbol(shipId);
    else if (dir == HORIZONTAL) //Horizontal
        for (int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId) ; c++)
            m_grid[topOrLeft.r][c] = m_game.shipSymbol(shipId);
    
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    // Return false if ShipId negative or more than stored
    if (shipId < 0 || shipId >= m_game.nShips())
        return false;
    // Ship wasn't placed on the board in the first place
    if (!shipsPlaced[shipId])
        return false;
    // If the full ship was not there, then cannot be unplaced
    if (dir == VERTICAL){
        for (int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId) ; r++)
            if (m_grid[r][topOrLeft.c] != m_game.shipSymbol(shipId))
                return false;
    }
    else if (dir == HORIZONTAL){
        for (int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId) ; c++)
            if (m_grid[topOrLeft.r][c] != m_game.shipSymbol(shipId))
                return false;
    }
    
    //Ship unplaced, replace location of ship with '.', and return true
    shipsPlaced[shipId] = false;
    if (dir == VERTICAL)
        for (int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId) ; r++)
            m_grid[r][topOrLeft.c] = '.';
    else if (dir == HORIZONTAL)
        for (int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId) ; c++)
            m_grid[topOrLeft.r][c] = '.';
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    //Print first line of column index
    cout << "  ";
    for (int i = 0; i < m_game.cols(); i++ )
        cout << i << " ";
    cout << endl;
    //Print row index, and the grids
    for (int r = 0; r < m_game.rows(); r++)
    {
        cout << r << " ";
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (shotsOnly) // Print just o and x if shotsOnly = true
            {
                if (m_grid[r][c] == 'o' ||  m_grid[r][c]  == 'X')
                    cout << m_grid[r][c] << " ";
                else
                    cout << "." << " ";
            }
            else //Print everything
                cout << m_grid[r][c] << " ";
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
 
    shipDestroyed = false;
    shotHit = false;
    
    // If attack is outside area return false
    if (!m_game.isValid(p))
        return false;
    // If attack on previously attacked cell return false
    if (m_grid[p.r][p.c] == 'o' || m_grid[p.r][p.c] == 'X' )
        return false;
    
    
    
    // If the ocean was hit, attack missed
    if (m_grid[p.r][p.c] == '.')
    {
        m_grid[p.r][p.c] = 'o';
        return true;
    }
    //If part of an undamaged ship was attacked
    else
    {
        shotHit = true;
        char ship_symbol;
        ship_symbol = m_grid[p.r][p.c];
        m_grid[p.r][p.c] = 'X';
        //Search entire grid, if one of the cells have the same symbol, entire ship not destroyed, return true
        for (int r = 0; r < m_game.rows(); r ++)
            for (int c = 0; c < m_game.cols(); c++)
                if (m_grid[r][c] == ship_symbol)
                    return true;
    
        // The ship must have been destroyed
        shipDestroyed = true;
        //Find the shipID by checking symbol of each ID from 0 until a match is found.
        for (int i = 0; i < m_game.nShips(); i++)
            if (m_game.shipSymbol(i) == ship_symbol)
            {
                shipId = i;
                break;
            }
        return true;
    }
}

bool BoardImpl::allShipsDestroyed() const
{
    //Check every cell of grid, return false if a ship symbol was found
    for (int r = 0; r < m_game.rows(); r ++)
        for (int c = 0; c < m_game.cols(); c++)
         if (m_grid[r][c] != 'o' && m_grid[r][c] != 'X' && m_grid[r][c] != '.')
             return false;
    //If no ship symbols were found in any cell, all ships must have been destroyed 
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
