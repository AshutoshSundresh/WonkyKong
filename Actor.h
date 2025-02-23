#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

static const int FROZEN_DURATION = 50;
static const int BURP_LIFETIME = 5;
static const int MOVEMENT_TICK_INTERVAL = 10;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;  // forward declaration

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

protected:
    bool m_canBeAttacked;

private:
    StudentWorld* m_world;
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
          m_isJumping(false), m_jumpStep(0), m_burpCount(0), m_isFrozen(false), m_frozenTicks(0) {}
    
    virtual void doSomething() override;
    
    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    virtual void attack() override;
    
    int getBurpCount() const { return m_burpCount; }
    void setBurpCount(int count) { m_burpCount = count; }
    void decrementBurpCount() { if (m_burpCount > 0) m_burpCount--; }
    void addBurp() { m_burpCount++; }
    
    bool isFrozen() const { return m_isFrozen; }
    void setFrozen(bool frozen) { 
        m_isFrozen = frozen; 
        if (frozen) m_frozenTicks = FROZEN_DURATION; 
    }
    
private:
    bool m_isAlive;
    bool m_isJumping;
    int m_jumpStep;
    int m_burpCount;
    bool m_isFrozen;
    int m_frozenTicks;
    
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
        : Actor(world, IID_BURP, startX, startY, dir), m_lifetime(BURP_LIFETIME) {}
    
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

// Base class for all enemies 
class Enemy : public Actor {
public:
    Enemy(StudentWorld* world, int imageID, int startX, int startY, int dir = none, double size = 1.0, bool canBeAttacked = false)
        : Actor(world, imageID, startX, startY, dir, size, canBeAttacked) {}
    virtual void doSomething() override;
    virtual void attack() override;  // base implementation for attack
    
protected:
    virtual void doEnemySpecificAction() = 0;  // pure virtual function for enemy-specific behavior
    virtual void onAttackBonus() {}  // non-pure virtual function for koopa and fireball
    void checkAndHandlePlayerCollision();  // common collision handling for all enemies
};

// represents a bonfire that can attack the player and barrels
class Bonfire : public Enemy {
public:
    Bonfire(StudentWorld* world, int startX, int startY)
        : Enemy(world, IID_BONFIRE, startX, startY) {}
    
protected:
    virtual void doEnemySpecificAction() override;
};

// represents a Koopa enemy that can freeze the player
class Koopa : public Enemy {
public:
    Koopa(StudentWorld* world, int startX, int startY)
        : Enemy(world, IID_KOOPA, startX, startY, (randInt(0, 1) == 0) ? left : right, 1.0, true), m_freezeCooldown(0), m_moveCounter(0) {}
    
    virtual void doSomething() override;  // Override base doSomething for freeze implementation
    
protected:
    virtual void onAttackBonus() override;
    virtual void doEnemySpecificAction() override;

private:
    int m_freezeCooldown;  // cooldown timer for freeze attack
    int m_moveCounter;     // counter for movement timing
};

// represents a Fireball enemy that moves back and forth and can climb ladders
class Fireball : public Enemy {
public:
    Fireball(StudentWorld* world, int startX, int startY)
        : Enemy(world, IID_FIREBALL, startX, startY, (randInt(0, 1) == 0) ? left : right, 1.0, true),
          m_climbingUp(false), m_climbingDown(false), m_justGotOffLadder(false), m_tickCount(0) {}
        
    virtual void doSomething() override;  // skip base class collision check

protected:
    virtual void onAttackBonus() override;
    virtual void doEnemySpecificAction() override;

private:
    bool m_climbingUp;        
    bool m_climbingDown;
    bool m_justGotOffLadder;
    int m_tickCount;
    
    int getXMod() const { return getDirection() == right ? 1 : -1; }
};

// represents a rolling barrel that can attack the player
class Barrel : public Enemy {
public:
    Barrel(StudentWorld* world, int startX, int startY, int dir)
        : Enemy(world, IID_BARREL, startX, startY, dir, 1.0, true), m_tickCount(0) {}
    
    virtual void doSomething() override;  // skip base class collision check
        
protected:
    virtual void doEnemySpecificAction() override;

private:
    int m_tickCount;  
};

// represents Kong, who throws barrels and flees when player gets close
class Kong : public Enemy {
public:
    Kong(StudentWorld* world, int startX, int startY, int dir)
        : Enemy(world, IID_KONG, startX, startY, dir == 1 ? right : left, 1.0, false), 
          m_isFleeing(false), m_ticksSinceLastBarrel(0), m_tickCount(0) {}
          
    virtual void doSomething() override;

protected:
    virtual void doEnemySpecificAction() override {}

private:
    bool m_isFleeing;
    int m_ticksSinceLastBarrel;
    int m_tickCount;

    double getDistanceToPlayer() const;
    int getBarrelThrowInterval() const;
};

#endif // ACTOR_H_
