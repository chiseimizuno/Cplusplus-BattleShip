#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
    {
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    }
    return true;
}


Point AwfulPlayer::recommendAttack()
{
    //Attacks from bottom right corner, work way left then up
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}


//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
// typedef AwfulPlayer HumanPlayer;

class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){}
    virtual void recordAttackByOpponent(Point p) {}
private:
    Point m_lastCellAttacked;
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{
}



bool HumanPlayer::placeShips(Board& b)
{
    
    cout << name() << " must place " << game().nShips() << " ships" << endl;
    
    //Loop until all ships are placed
    for (int i = 0; i < game().nShips(); i++)
    {
        //Initialization
        int r = 0;
        int c = 0;
        string dir = " ";
        b.display(false);
        //Get directionality until usuer inputs a valid character
        while (dir != "h" && dir != "v")
        {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            cin >> dir;
            if (dir != "h" && dir != "v")
            {
                cout << "Direction must be h or v." << endl;
                cin.clear();  // clear error state so can do more input operations
                cin.ignore(10000, '\n');
                
            }
        }
        //Ask user for 2 valid integers
        while (1)
        {
            cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
            while (!getLineWithTwoIntegers(r, c))
            {
                cout << "You must enter 2 integers" << endl;
                cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
            }
            
            //Place ships depending on user input
            if (dir == "h")
            {
                if (b.placeShip(Point(r,c), i, HORIZONTAL))
                    break;
                else
                    cout << "The ship can not be placed there." << endl;
            }
            if (dir == "v")
            {
                if (b.placeShip(Point(r,c), i, VERTICAL))
                    break;
                else
                    cout << "The ship can not be placed there." << endl;
            }
        }
        
        
        
    }
    return true;
    
}

Point HumanPlayer::recommendAttack()
{
    //Loop until user input is accepted format
    while (1)
    {
        int r, c;
        cout << "Enter the row and column to attack (e.g, 3 5): ";
        
        //Makes sure user inputs 2 integers
        while (!getLineWithTwoIntegers(r, c))
            cout << "You must enter 2 integers" << endl;
        
        return Point(r,c);
        
    }
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************


class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p) {}
private:
    int shipState = 1;
    Point m_lastCellAttacked;
    vector<Point> history;
    bool placeShipsRecursive(Board &b, Point topOrLeft, int shipId);
    Point incrementPoint(Point p);
    bool notDestroyed(Point p);
    bool findInHistory(Point p);
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{
}


bool MediocrePlayer::placeShips(Board &b)
{
    //Keep trying to place ship on board with different block configuration
    for (int i = 0; i < 50; i++)
    {
        b.block();
        if (placeShipsRecursive(b, Point(0,0), 0))
        {
            b.unblock();
            return true;
        }
        b.clear();
    }
    return false;
}

//Increment Point, from left to right, then top to bottom
Point MediocrePlayer::incrementPoint(Point p)
{
    if (p.c == game().cols() - 1)
        return Point(p.r+1, 0);
    else
        return Point(p.r, p.c+1);
}

//New member function to place ships recursively
bool MediocrePlayer::placeShipsRecursive(Board &b,Point topOrLeft, int shipId){
    //All ships have been placed
    if (shipId == game().nShips())
        return true;
    
    //If point reaches bottom right corner, cannot be placed
    if (topOrLeft.c == game().cols()-1 && topOrLeft.r == game().rows() - 1)
        return false;
    
    //Try to place Horizontally
    if ( b.placeShip(topOrLeft, shipId, HORIZONTAL) )
        if (placeShipsRecursive(b, Point(0,0), shipId +1)) //See if next ship can be placed
            return true;
    b.unplaceShip(topOrLeft, shipId, HORIZONTAL); //Unplace ships if failed
    
    //Try to place Vertically
    if (b.placeShip(topOrLeft, shipId, VERTICAL))
        if (placeShipsRecursive(b, Point(0,0), shipId +1)) //See if next ship can be placed
            return true;
    b.unplaceShip(topOrLeft, shipId, VERTICAL); //Unplace ships if failed
    
    //Call function again, but increment location by one
    Point newTopOrLeft = incrementPoint(topOrLeft);
    return (placeShipsRecursive(b, newTopOrLeft, shipId));
    
}

//Checks if point exists in history of attack locations
bool MediocrePlayer::findInHistory(Point p)
{
    for (int i = 0; i < history.size(); i++)
        if (p.c == history[i].c && p.r == history[i].r)
            return true;
    return false;
}

//If ship hasn't been destroyed after checking all possible locations
bool MediocrePlayer::notDestroyed(Point p)
{
    Point checkCol;
    Point checkRow;
    for (int i = 0; i < 9; i++)
    {
        //Check vertically
        checkCol = Point(m_lastCellAttacked.r + 4 - i,m_lastCellAttacked.c);
        if (game().isValid(checkCol) && !findInHistory(checkCol))
            return false;
        
        //Check horizontally
        checkRow = Point(m_lastCellAttacked.r ,m_lastCellAttacked.c + 4 - i);
        if (game().isValid(checkRow) && !findInHistory(checkRow))
            return false;
    }
    return true;
}

Point MediocrePlayer::recommendAttack()
{
    //If all points 4 steps adjacent to the last cell check has been checked, and still no ship destroyed
    if (notDestroyed(m_lastCellAttacked))
        shipState = 1;
    Point attackNext;
    
    //State 1: has not hit ship yet, attack random non-redundant locations
    if (shipState == 1)
    {
        while (1)
        {
            attackNext = game().randomPoint();
            if (!findInHistory(attackNext))
                return attackNext;
        }
        
    }
    
    //State 2: a shit has been hit, randomly hit up to 4 adjacent cells in all 4 directions
    if (shipState == 2)
    {
        while(1)
        {
            if (randInt(2) == 0)
                attackNext = Point(m_lastCellAttacked.r + 4 - randInt(9),m_lastCellAttacked.c);
            else
                attackNext = Point(m_lastCellAttacked.r ,m_lastCellAttacked.c + 4 - randInt(9));
            
            //If point is valid (i.e. inside board) or if point hasn't already been attacked
            if (!game().isValid(attackNext))
                continue;
            if (!findInHistory(attackNext))
                return attackNext;
        }
    }
    
    return Point(0,0); 
}

//Record the result of the attack
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    history.push_back(p); //Keep track of the location that was attacked
    
    //While still in attack random mode
    if (shipState == 1)
    {
        if (validShot == false || shotHit == false) //If shot was invalid, or in ocean
            return;
        if (validShot == true && shotHit == true && shipDestroyed == true) //If shot hit and destroyed something
            return;
        if (validShot == true && shotHit == true && shipDestroyed == false) //If shot hit but not destroyed
        {
            //Set to state 2, where we plan an attack around the location that was hit
            m_lastCellAttacked = p;
            shipState = 2;
            return;
        }
    }
    
    //Planned attack mode
    if (shipState == 2)
    {
        if (validShot == false || shotHit == false) //If shot was invalid or did not hit anything
            return;
        if (validShot == true && shotHit == true && shipDestroyed == false) //If ship hit but not destroyed
            return;
        if (validShot == true && shotHit == true && shipDestroyed == true) //If ship destroyed
        {
            shipState = 1;
            return;
        }
        
        
    }
    
        
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p) {}
private:
    int numAtt = 0;
    int m_ship[10][10]; //Own history of location of ships placed
    int shipState = 1;
    vector<Point> history;
    bool shipPlacement(Board& b, int n, int shipId);
    bool placeShipsRecursive(Board &b,Point topOrLeft, int shipId);
    bool canPlace(Point p, int shipId, Direction dir);
    bool findInHistory(Point p);
    void unplace(Point p, int shipId, Direction dir);
    void clear();
    Point m_lastCellAttacked;
    Point incrementPoint(Point p);
};


GoodPlayer::GoodPlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{
    for (int r = 0; r < 10; r++)
        for (int c = 0; c < 10; c++)
            m_ship[r][c] = -1; //Sets personal grid to be all -1
}

//Clears personal grid
void GoodPlayer::clear()
{
    for (int r = 0; r < 10; r++)
        for (int c = 0; c < 10; c++)
            m_ship[r][c] = -1; //Personal grid = -1
}

bool GoodPlayer::placeShips(Board &b)
{
    //Try to place ship 50 times with different locations blocked each time
    for (int i = 0; i < 50; i++)
    {
        b.block();
        if (placeShipsRecursive(b, Point(0,0), 0))
        {
            b.unblock();
            return true;
        }
        b.clear();
        clear();
    }
    return false;
}

//Increment location of point by one grid (left to right, top to bottom)
Point GoodPlayer::incrementPoint(Point p)
{
    if (p.c == game().cols() - 1)
        return Point(p.r+1, 0);
    else
        return Point(p.r, p.c+1);
}

//A function that sees if there are any ships that are exactly 4 steps vertical or horizontal from the
//location of the current ship being placed
bool GoodPlayer::canPlace(Point p, int shipId, Direction dir)
{
    //Placing ship on our personal grid
    int shipLength = game().shipLength(shipId);
    
    if (dir == HORIZONTAL)
        for (int i = 0; i < shipLength; i ++)
            m_ship[p.r][p.c + i] = shipId;
    
    if (dir == VERTICAL)
        for (int i = 0; i < shipLength; i ++)
            m_ship[p.r + i][p.c] = shipId;
    
    //Check 4 positions vertically and horizontally, for each location of one ship
    Point originalP = p;
    for (int i = 0; i < shipLength; i++)
    {
        //In each iteration, check the different positions occupied by the ship
        if (dir == HORIZONTAL)
            p = Point(originalP.r,originalP.c+i);
        else
            p = Point(originalP.r+i,originalP.c);
        
        //Check 4 grids directly to the right and left
        for (int j = 0; j < 9; j++)
        {
            if (p.c-4+j < 0 || p.c-4+j >= game().cols())
                continue;
            if (m_ship[p.r][p.c-4+j] != -1 && m_ship[p.r][p.c-4+j] != shipId)
                return false;
        }
        
        //Check 4 grids directly to the top and bottom
        for ( int j = 0; j < 9; j++)
        {
            if (p.r-4+j < 0 || p.r-4+j >= game().rows())
                continue;
            if (m_ship[p.r-4+j][p.c] != -1 && m_ship[p.r-4+j][p.c] != shipId)
                return false;
        }
        
    }
    return true;
    
}

//Unplace ships indicated by the parameter
void GoodPlayer::unplace(Point p, int shipId, Direction dir)
{
    if (dir == HORIZONTAL){
        for (int i = 0; i < game().shipLength(shipId); i ++)
            if (game().isValid(Point(p.r,p.c+i)))
                if (m_ship[p.r][p.c+i] == shipId)
                    m_ship[p.r][p.c+i] = -1;
    }
    else
        for (int i = 0; i < game().shipLength(shipId); i ++)
            if (game().isValid(Point(p.r+i,p.c)))
                if (m_ship[p.r+i][p.c] == shipId)
                    m_ship[p.r+i][p.c] = -1;
}


//Place ships recursively
bool GoodPlayer::placeShipsRecursive(Board &b,Point topOrLeft, int shipId){
    //if all ships have been placed, return true
    if (shipId == game().nShips())
        return true;
    
    //If point has reached bottom right corner of grid, ship placement unsuccessful, return false
    if (topOrLeft.c == game().cols()-1 && topOrLeft.r == game().rows() - 1)
        return false;
    
    //If horizontal ship can be placed and not within 4 units of another ship
    if ( b.placeShip(topOrLeft, shipId, HORIZONTAL) && canPlace(topOrLeft, shipId, HORIZONTAL))
        if (placeShipsRecursive(b, Point(0,0), shipId +1)) //Place next ship
            return true;
    b.unplaceShip(topOrLeft, shipId, HORIZONTAL); //If unsuccessful, remove ships from board
    unplace(topOrLeft, shipId, HORIZONTAL); //If unsuccessful, remove ships from personal ship
    
    //Repeat for vertical ships
    if (b.placeShip(topOrLeft, shipId, VERTICAL) && canPlace(topOrLeft, shipId, VERTICAL) )
        if (placeShipsRecursive(b, Point(0,0), shipId +1))
            return true;
    b.unplaceShip(topOrLeft, shipId, VERTICAL);
    unplace(topOrLeft, shipId, VERTICAL);
    
    //Increment location of ship by one unit
    Point newTopOrLeft = incrementPoint(topOrLeft);
    return (placeShipsRecursive(b, newTopOrLeft, shipId));
    
}

//True if the point has already been attacked
bool GoodPlayer::findInHistory(Point p)
{
    for (int i = 0; i < history.size(); i++)
        if (p.c == history[i].c && p.r == history[i].r)
            return true;
    return false;
}

Point GoodPlayer::recommendAttack()
{
    
    Point attackNext;
    
    //In random attack mode, attacks even squares
    if (shipState == 1)
    {
        int evenOdd = 0;
        int count = 0;
        while (1)
        {
            //For the rare case that somehow no ship can be found in even cells of board, change from even to odd cells
            count ++;
            if (count > 50)
                evenOdd = 1;
            
            attackNext = game().randomPoint();
            //If attack is even square, and has not been attacked before
            if (!findInHistory(attackNext) && (attackNext.r + attackNext.c)%2 == evenOdd )
                return attackNext;
        }
        
    }
    
    //In alert mode, checks one unit in each direction to see if a ship is there
    if (shipState == 2)
    {
        int count = 0;
        int dist1 = 1;
        int dist2 = 3;

        while(1)
        {
            count ++;
            //If the even-squares were checked 50 times, choose squares randomly
            if (count > 50)
            {
                dist1 = dist1 + 1;
                dist2 = dist2 + 2;
                count = 0;
            }
            //Attack randomly N,E,S,W, or itself
            if (randInt(2) == 0)
                attackNext = Point(m_lastCellAttacked.r , m_lastCellAttacked.c + dist1  - randInt(dist2));
            else
                attackNext = Point(m_lastCellAttacked.r + dist1 - randInt(dist2),m_lastCellAttacked.c);
            
            //If attack location is valid and has not previously been attacked
            if (!game().isValid(attackNext))
                continue;
            if (!findInHistory(attackNext))
                return attackNext;
            
        }
    }
    
    //In attack mode, either continuously attacks right or bottom
    if (shipState == 3)
    {
        //If the 2nd to the last and most recent attack were on the same row, continue attacking on same row
        if (history[history.size()-1].r == m_lastCellAttacked.r )
        {
            int i = 0;
            while(1)
            {
                attackNext = Point(m_lastCellAttacked.r, m_lastCellAttacked.c + i);
                i = i + 1; //Keep going right
                if (!game().isValid(attackNext))
                {
                    shipState = 4;
                    break;
                }
                if (!findInHistory(attackNext))
                    return attackNext;
            }
        }
        
        //If the 2nd to the last and most recent attack were on the same column, continue attacking on same column
        if (history[history.size()-1].c == m_lastCellAttacked.c )
        {
            int i = 0;
            while(1)
            {
                attackNext = Point(m_lastCellAttacked.r + i, m_lastCellAttacked.c ); //Keep attacking
                i = i + 1; //Continously check left
                if (!game().isValid(attackNext))
                {
                    shipState = 4;
                    break;
                }
                if (!findInHistory(attackNext))
                    return attackNext;
            }
        }
    }
    
    //Repeating as above, except in opposite directions
    if (shipState == 4)
    {
        if (history[history.size()-1].r == m_lastCellAttacked.r )
        {
            int i = 0;
            while(1)
            {
                attackNext = Point(m_lastCellAttacked.r, m_lastCellAttacked.c - i);
                i = i + 1; //Go left
                if (!game().isValid(attackNext))
                {
                    shipState = 2;
                    break;
                }
                if (!findInHistory(attackNext))
                    return attackNext;
            }
        }
        
        if (history[history.size()-1].c == m_lastCellAttacked.c )
        {
            int i = 0;
            while(1)
            {
                attackNext = Point(m_lastCellAttacked.r - i, m_lastCellAttacked.c);
                i = i + 1; // Go top
                if (!game().isValid(attackNext))
                {
                    shipState = 2;
                    break;
                }
                if (!findInHistory(attackNext))
                    return attackNext;
            }
        }
    }
    
    //In alert mode. There must be another ship nearby, but not exactly sure where
    if (shipState == 5)
    {
        int count = 0;
        int dist1 = 1;
        int dist2 = 3;
        while (1)
        {
            count++;
            if (count > 30)
            {
                dist1++;
                dist2 = dist2 + 2;
                if (dist1 == 4)
                {
                    shipState = 1;
                    break;
                }
            }
            //Randomly attack nearby locations in hopes of finding a ship
            attackNext = Point( m_lastCellAttacked.r - dist1 + randInt(dist2), m_lastCellAttacked.c - dist1 + randInt(dist2));
            if (!game().isValid(attackNext))
                continue;
            if (!findInHistory(attackNext))
                return attackNext;
        }
    }
    
    return recommendAttack(); //Go back to the beginning and try to do it again
}

//Records result of attack
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    //If shot hit something, keep track of number of attacks
    if (validShot && shotHit)
        numAtt++;
    
    //Keep track of location of attacks
    history.push_back(p);
    
    //If player was in random mode
    if (shipState == 1)
    {
        if (validShot == false || shotHit == false)
            return;
        if (validShot == true && shotHit == true && shipDestroyed == true)
            return;
        if (validShot == true && shotHit == true && shipDestroyed == false)
        {
            //Change to alert mode if a ship was hit and not destroyed
            m_lastCellAttacked = p;
            shipState = 2;
            return;
        }
    }
    
    //If player was in alert mode
    if (shipState == 2)
    {
        if (validShot == false || shotHit == false)
            return;
        if (validShot == true && shotHit == true && shipDestroyed == false)
        {
            //Change to attack mode if another location of ship attacked but not destroyed
            shipState = 3;
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == true)
        {
            //If ship was hit and destroyed, but attacked more than the length of ship, search for the nearby ship (state 5)
            if (numAtt > game().shipLength(shipId))
                shipState = 5;
            else
                shipState = 1;
            numAtt = 0;
            return;
        }
    }
    
    // If player was in attack mode, goes only one direction
    if (shipState == 3)
    {
        if (validShot == false || shotHit == false)
        {
            //If going one direction doesn't work, go the other direction
            shipState = 4;
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == false)
        {
            //If shot hit, keep going the same direction
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == true)
        {
            //If ship was hit and destroyed, but attacked more than the length of ship, search for the nearby ship (state 5)
            if (numAtt > game().shipLength(shipId))
                shipState = 5;
            else
                shipState = 1;
            numAtt = 0;
            return;
        }
        
    }
    
    //If player was in attack mode, go the other direction of state 3
    if (shipState == 4)
    {
        if (validShot == false || shotHit == false)
        {
            //If both 3 and 4 fails, probably should go back to state 2, look other directions
            shipState = 2;
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == false)
        {
            //If shot successful, keep going!
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == true)
        {
            //If ship was hit and destroyed, but attacked more than the length of ship, search for the nearby ship (state 5)
            if (numAtt > game().shipLength(shipId))
                shipState = 5;
            else
                shipState = 1; //Unless the number of ships hit didn't match length of size
            numAtt = 0;
            return;
        }
        
    }
    
    if (shipState == 5)
    {
        if (validShot == false || shotHit == false)
        {
            //If shot unsuccessful, keep going, the ship should still be nearby
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == false)
        {
            //Ship founded! go back to state 2 or alert mode and attack! 
            shipState = 2;
            m_lastCellAttacked = p;
            return;
        }
        if (validShot == true && shotHit == true && shipDestroyed == true)
        {
            //If ship was hit and destroyed, but attacked more than the length of ship, search for the nearby ship (state 5)
            if (numAtt > game().shipLength(shipId))
                shipState = 5;
            else
                shipState = 1; //Unless the number of ships hit didn't match length of size
            numAtt = 0;
            return;
        }
        
    }
    
}


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}


