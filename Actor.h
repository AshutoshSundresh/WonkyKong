#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;  // forward declaration

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = none, double size = 1.0)
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
        : Actor(world, IID_FLOOR, startX, startY, none) {}  // no direction
    
    virtual void doSomething() override {}  // floor doesn't do anything each tick
};

// represents the player character
class Player : public Actor {
public:
    Player(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_PLAYER, startX, startY, right), m_lives(3), m_isAlive(true), 
          m_isJumping(false), m_jumpStep(0) {}
    
    virtual void doSomething() override;
    
    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    int getLives() const { return m_lives; }
    void decrementLives() { m_lives--; }
    
private:
    int m_lives;
    bool m_isAlive;
    bool m_isJumping;
    int m_jumpStep;
    
    void executeJumpStep();
    bool canInitiateJump() const;
    void handleKeyPress();
    void handleFalling();
};

// represents a ladder that the player can climb
class Ladder : public Actor {
public:
    Ladder(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_LADDER, startX, startY, none) {}
    
    virtual void doSomething() override {}  // ladder is static, doesn't do anything each tick
};

#endif // ACTOR_H_
