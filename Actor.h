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
        : Actor(world, IID_PLAYER, startX, startY, right), m_isAlive(true), 
          m_isJumping(false), m_jumpStep(0), m_burpCount(0) {}
    
    virtual void doSomething() override;
    
    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    virtual void attack() override;
    
    int getBurpCount() const { return m_burpCount; }
    void setBurpCount(int count) { m_burpCount = count; }
    void decrementBurpCount() { if (m_burpCount > 0) m_burpCount--; }
    void addBurp() { m_burpCount++; }
    
private:
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

class Goodie : public Actor {
public:
    Goodie(StudentWorld* world, int imageID, int startX, int startY)
        : Actor(world, imageID, startX, startY, none) {}
    
    virtual void doSomething() override final;  
    virtual void attack() override {} // goodies can't be attacked
    
protected:
    virtual int getScoreIncrease() const = 0;
    virtual void doGoodieSpecificAction() = 0;
};

// represents a garlic goodie that gives the player burps
class GarlicGoodie : public Goodie {
public:
    GarlicGoodie(StudentWorld* world, int startX, int startY)
        : Goodie(world, IID_GARLIC_GOODIE, startX, startY) {}
    
protected:
    virtual int getScoreIncrease() const override { return 25; }
    virtual void doGoodieSpecificAction() override;
};

// represents an extra life goodie that gives the player an extra life
class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY)
        : Goodie(world, IID_EXTRA_LIFE_GOODIE, startX, startY) {}
    
protected:
    virtual int getScoreIncrease() const override { return 50; }
    virtual void doGoodieSpecificAction() override;
};

// Base class for all enemies that can kill the player
class Enemy : public Actor {
public:
    Enemy(StudentWorld* world, int imageID, int startX, int startY, int dir = none, double size = 1.0)
        : Actor(world, imageID, startX, startY, dir, size, false) {}
    virtual void doSomething() override;
    
protected:
    virtual void doEnemySpecificAction() = 0;  // pure virtual function for enemy-specific behavior
};

// represents a bonfire that can attack the player and barrels
class Bonfire : public Enemy {
public:
    Bonfire(StudentWorld* world, int startX, int startY)
        : Enemy(world, IID_BONFIRE, startX, startY) {}
    
protected:
    virtual void doEnemySpecificAction() override;
};

#endif // ACTOR_H_
