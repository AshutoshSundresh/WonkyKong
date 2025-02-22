#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool isBlockedByFloor(int x, int y) const;
    bool isOnLadder(int x, int y) const;  // returns true if there is a ladder at (x,y)
    void playSound(int soundId);

private:
    void setDisplayText();  // helper function to set the game stats display
    std::vector<Actor*> m_actors;  // container for all actors (Floor, Player, etc.)
    Player* m_player;  
    Level m_level;     
};

#endif // STUDENTWORLD_H_
