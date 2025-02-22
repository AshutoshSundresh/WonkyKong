#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;  // forward declaration

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = none, double size = 1.0, bool canBeAttacked = true)
        : GraphObject(imageID, startX, startY, dir, size), m_world(world), m_canBeAttacked(canBeAttacked), m_isAlive(true) {}
    
    virtual void doSomething() = 0;  // pure virtual
    
    StudentWorld* getWorld() const { return m_world; }
    bool canBeAttacked() const { return m_canBeAttacked; }
    virtual void attack() {} // Default implementation does nothing
    
    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    
    virtual ~Actor() {} 

private:
    StudentWorld* m_world;
    bool m_canBeAttacked;
    bool m_isAlive;
};

// static floor pieces in the game
class Floor : public Actor {
public:
    Floor(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_FLOOR, startX, startY, none, 1.0, false) {}  // floor can't be attacked
    
    virtual void doSomething() override {}  // floor doesn't do anything each tick
};

// represents the player character
class Player : public Actor {
public:
    Player(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_PLAYER, startX, startY, right), m_lives(3), m_isAlive(true), 
          m_isJumping(false), m_jumpStep(0), m_burpCount(0) {}
    
    virtual void doSomething() override;
    
    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    int getLives() const { return m_lives; }
    void decrementLives() { m_lives--; }
    
    int getBurpCount() const { return m_burpCount; }
    void setBurpCount(int count) { m_burpCount = count; }
    void decrementBurpCount() { if (m_burpCount > 0) m_burpCount--; }
    void addBurp() { m_burpCount++; }
    
private:
    int m_lives;
    bool m_isAlive;
    bool m_isJumping;
    int m_jumpStep;
    int m_burpCount;
    
    void executeJumpStep();
    bool canInitiateJump() const;
    void handleKeyPress();
    void handleFalling();
    void handleBurp();
};

// represents a ladder that the player can climb
class Ladder : public Actor {
public:
    Ladder(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_LADDER, startX, startY, none, 1.0, false) {}  // ladder can't be attacked
    
    virtual void doSomething() override {}  // ladder is static, doesn't do anything each tick
};

// represents a burp attack
class Burp : public Actor {
public:
    Burp(StudentWorld* world, int startX, int startY, int dir)
        : Actor(world, IID_BURP, startX, startY, dir), m_lifetime(5) {}
    
    virtual void doSomething() override;
    virtual void attack() override {} // Burp can't be attacked

private:
    int m_lifetime;
};

// represents a garlic goodie that gives the player burps
class GarlicGoodie : public Actor {
public:
    GarlicGoodie(StudentWorld* world, int startX, int startY)
        : Actor(world, IID_GARLIC_GOODIE, startX, startY, none) {}
    
    virtual void doSomething() override;
    virtual void attack() override {} // Garlic goodie can't be attacked
};

#endif // ACTOR_H_
