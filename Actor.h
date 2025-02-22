#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;  // forward declaration

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = 0, double size = 1.0)
        : GraphObject(imageID, startX, startY, dir, size), m_world(world) {}
    
    virtual void doSomething() = 0;  // pure virtual
    
    StudentWorld* getWorld() const { return m_world; }
    
    virtual ~Actor() {} 

private:
    StudentWorld* m_world;
};

// static floor pieces in the game
class Floor : public Actor {
public:
    Floor(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_FLOOR, startX, startY) {}
    
    virtual void doSomething() override {}  // floor doesn't do anything each tick
};

// represents the player character
class Player : public Actor {
public:
    Player(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_PLAYER, startX, startY) {}
    
    virtual void doSomething() override; 
};

#endif // ACTOR_H_
