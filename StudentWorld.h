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
    void addActor(Actor* actor) { m_actors.push_back(actor); }
    void attackNonPlayerActorsAt(int x, int y);  // attacks only non-player actors at (x,y)
    bool isPlayerAt(int x, int y) const { return m_player->getX() == x && m_player->getY() == y; }
    Player* getPlayer() const { return m_player; }
    void freezePlayer() { m_player->setFrozen(true); }  

    void getPlayerLocation(int& x, int& y) const { x = m_player->getX(); y = m_player->getY(); }
    void attackPlayer() { m_player->attack(); }
    void attackBarrelsAt(int x, int y);  // attacks only barrels at the given position
    void levelComplete() { m_levelComplete = true; }
    bool isLevelComplete() const { return m_levelComplete; }

private:
    void setDisplayText();  // helper function to set the game stats display
    std::vector<Actor*> m_actors;  // container for all actors (Floor, Player, etc.)
    Player* m_player;  
    Level m_level;     
    bool m_levelComplete;  
};

#endif // STUDENTWORLD_H_
