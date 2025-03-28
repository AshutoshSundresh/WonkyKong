#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Player::doSomething() {
    if (!isAlive())
        return;
    
    // Handle frozen state
    if (m_isFrozen) {
        m_frozenTicks--;
        if (m_frozenTicks <= 0) {
            m_isFrozen = false;
        }
        return;  // Do nothing else while frozen
    }
        
    if (m_isJumping) {
        executeJumpStep();
        return;
    }
    
    // Check for falling
    if (!getWorld()->isBlockedByFloor(getX(), getY() - 1) && 
        !getWorld()->isOnLadder(getX(), getY()) &&
        !getWorld()->isOnLadder(getX(), getY() - 1)) {
        moveTo(getX(), getY() - 1);  // fall one square
        return;
    }
    
    handleKeyPress();
}

void Player::executeJumpStep() {
    int targetX = getX();
    int targetY = getY();
    
    switch (m_jumpStep) {
        case 0:  // Move up
            targetY++;
            break;
        case 1:  // Move in facing direction
        case 2:
        case 3:
            targetX += getXMod();
            break;
        case 4:  // Move down
            targetY--;
            break;
    }
    
    // Check if movement is blocked or would go off screen
    if (targetX < 0  || targetY < 0 ||
        getWorld()->isBlockedByFloor(targetX, targetY)) {
        m_isJumping = false;
        return;
    }
    
    // Move to new position
    moveTo(targetX, targetY);
    
    // Check if landed on ladder
    if (getWorld()->isOnLadder(targetX, targetY)) {
        m_isJumping = false;
        return;
    }
    
    // Increment jump step or end jump sequence
    m_jumpStep++;
    if (m_jumpStep >= 5)
        m_isJumping = false;
}

bool Player::canInitiateJump() const {
    // can jump if standing on floor/ladder or currently on ladder or ladder below
    return getWorld()->isBlockedByFloor(getX(), getY() - 1) ||
           getWorld()->isOnLadder(getX(), getY()) ||
           getWorld()->isOnLadder(getX(), getY() - 1);
}

void Player::handleKeyPress() {
    int ch;
    if (!getWorld()->getKey(ch))
        return;
        
    int targetX = getX();
    int targetY = getY();
    
    switch (ch) {
        case KEY_PRESS_LEFT:
            if (getDirection() != left)
                setDirection(left);
            else if (!getWorld()->isBlockedByFloor(getX() - 1, getY()))
                moveTo(getX() - 1, getY());
            break;
            
        case KEY_PRESS_RIGHT:
            if (getDirection() != right)
                setDirection(right);
            else if (!getWorld()->isBlockedByFloor(getX() + 1, getY()))
                moveTo(getX() + 1, getY());
            break;
            
        case KEY_PRESS_UP:
            if (getWorld()->isOnLadder(getX(), getY()) && 
                !getWorld()->isBlockedByFloor(getX(), getY() + 1))
                moveTo(getX(), getY() + 1);
            break;
            
        case KEY_PRESS_DOWN:
            // Allow moving down if either:
            // 1. Player is currently on a ladder, or
            // 2. There is a ladder right below the player
            if ((getWorld()->isOnLadder(getX(), getY()) || 
                 getWorld()->isOnLadder(getX(), getY() - 1)) && 
                !getWorld()->isBlockedByFloor(getX(), getY() - 1))
                moveTo(getX(), getY() - 1);
            break;
            
        case KEY_PRESS_TAB:
            handleBurp();
            break;
            
        case KEY_PRESS_SPACE:
            if (!m_isJumping && canInitiateJump()) {
                m_isJumping = true;
                m_jumpStep = 0;
                getWorld()->playSound(SOUND_JUMP);
            }
            break;
    }
}

void Player::handleBurp() {
    if (m_burpCount <= 0)
        return;
        
    // create burp in front of player
    int burpX = getX() + getXMod();
    int burpY = getY();
    
    // play burp sound
    getWorld()->playSound(SOUND_BURP);
    
    // create and add burp to the world
    getWorld()->addActor(new Burp(getWorld(), burpX, burpY, getDirection()));
    
    // decrease burp count
    decrementBurpCount();
}

void Player::attack() {
    getWorld()->playSound(SOUND_PLAYER_DIE);
    getWorld()->decLives();
    setDead();
}

void Burp::doSomething() {
    // Check if alive
    if (!isAlive())
        return;
        
    // decrement lifetime
    m_lifetime--;
    if (m_lifetime <= 0) {
        setDead();
        return;
    }
    
    // attack any non-player attackable actor in the current square
    getWorld()->attackNonPlayerActorsAt(getX(), getY());
}

void Goodie::doSomething() {
    // check if alive
    if (!isAlive())
        return;
        
    // check if player is on same square
    if (getWorld()->isPlayerAt(getX(), getY())) {
        getWorld()->increaseScore(getScoreIncrease());
        getWorld()->playSound(SOUND_GOT_GOODIE);
        doGoodieSpecificAction();
        setDead();
    }
}

void GarlicGoodie::doGoodieSpecificAction() {
    getWorld()->getPlayer()->setBurpCount(getWorld()->getPlayer()->getBurpCount() + 5);
}

void ExtraLifeGoodie::doGoodieSpecificAction() {
    getWorld()->incLives();
}

void Enemy::doSomething() {
    if (!isAlive())
        return;
    checkAndHandlePlayerCollision();
    doEnemySpecificAction();
}

void Enemy::checkAndHandlePlayerCollision() {
    if (getWorld()->isPlayerAt(getX(), getY())) {
        getWorld()->getPlayer()->attack();
    }
}

void Enemy::attack() {
    if (!isAlive())
        return;

    getWorld()->playSound(SOUND_ENEMY_DIE);    
    setDead();
    getWorld()->increaseScore(100);
    
    onAttackBonus();
}

void Bonfire::doEnemySpecificAction() {
    increaseAnimationNumber();  // animate the flames
    
    // Check for and destroy any barrels at the same position
    getWorld()->attackBarrelsAt(getX(), getY());
}

// koopa can die unlike the base enemy doSomething, it also does not kill the player
void Koopa::doSomething() {
    if (!isAlive())
        return;
        
    // Check if on same square as player and can freeze
    if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
        getWorld()->freezePlayer();
        m_freezeCooldown = FROZEN_DURATION;  // Set cooldown timer
        return;
    }

    // Decrement freeze cooldown if active
    if (m_freezeCooldown > 0)
        m_freezeCooldown--;

    // Handle movement every MOVEMENT_TICK_INTERVAL ticks
    m_moveCounter = (m_moveCounter + 1) % MOVEMENT_TICK_INTERVAL;
    if (m_moveCounter == 0) {
        doEnemySpecificAction();
    }
}

void Koopa::doEnemySpecificAction() {
    // handle Koopa's movement here
    // Determine target position based on direction
    int targetX = getX() + getXMod();
    
    // check if movement is valid
    bool hasFloorAhead = getWorld()->isBlockedByFloor(targetX, getY());
    bool hasFloorBelow = getWorld()->isBlockedByFloor(targetX, getY() - 1);
    bool hasLadderBelow = getWorld()->isOnLadder(targetX, getY() - 1);
    
    // Can move if there's no floor ahead AND (there's floor OR ladder below)
    if (!hasFloorAhead && (hasFloorBelow || hasLadderBelow)) {
        moveTo(targetX, getY());
        
        if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
            getWorld()->freezePlayer();
            m_freezeCooldown = FROZEN_DURATION;
        }
    } else {
        // reverse direction
        setDirection(getDirection() == right ? left : right);
    }
}

void Koopa::onAttackBonus() {
    if (randInt(1, 3) == 1) {
        getWorld()->addActor(new ExtraLifeGoodie(getWorld(), getX(), getY()));
    }
}

bool Fireball::tryClimbing() {
    if (!m_justGotOffLadder) {
        // not just off ladder, check if we can climb
        if (getWorld()->isOnLadder(getX(), getY()) && 
            !getWorld()->isBlockedByFloor(getX(), getY() + 1) &&
            randInt(0, 2) == 0) {
            // climb up with 1/3 chance
            m_climbingUp = true;
            moveTo(getX(), getY() + 1);
            checkAndHandlePlayerCollision();
            m_tickCount = 0;
            return true;
        } else if (getWorld()->isOnLadder(getX(), getY() - 1) &&
                  randInt(0, 2) == 0) {
            // climb down with 1/3 chance
            m_climbingDown = true;
            moveTo(getX(), getY() - 1);
            checkAndHandlePlayerCollision();
            m_tickCount = 0;
            return true;
        }
    }
    return false; // Didn't climb
}

void Fireball::handleClimbingEnd(bool& climbingFlag) {
    climbingFlag = false;
    m_justGotOffLadder = true;
    
    // try to move in current direction
    int nextX = getX() + getXMod();
    if (!getWorld()->isBlockedByFloor(nextX, getY()) && 
        (getWorld()->isBlockedByFloor(nextX, getY() - 1) || 
         getWorld()->isOnLadder(nextX, getY() - 1))) {
        moveTo(nextX, getY());
    } else {
        // if can't move forward, reverse direction and try again next tick
        setDirection(getDirection() == right ? left : right);
    }
    checkAndHandlePlayerCollision();
    m_tickCount = 0;
}

bool Fireball::handleClimbing(bool isClimbingUp, bool& climbingFlag) {
    if (climbingFlag) {
        bool shouldEndClimbing;
        int yDirection = isClimbingUp ? 1 : -1;
        
        if (isClimbingUp) {
            // check if ladder ends or ceiling is hit when climbing up
            shouldEndClimbing = !getWorld()->isOnLadder(getX(), getY()) || 
                               getWorld()->isBlockedByFloor(getX(), getY() + 1);
        } else {
            // check if ladder ends when climbing down
            shouldEndClimbing = !getWorld()->isOnLadder(getX(), getY() - 1);
        }
        
        if (shouldEndClimbing) {
            handleClimbingEnd(climbingFlag);
        } else {
            // continue climbing in the appropriate direction
            moveTo(getX(), getY() + yDirection);
            checkAndHandlePlayerCollision();
            m_tickCount = 0;
        }
        return true; // handled the climbing
    }
    return false; // not climbing in this direction
}

void Fireball::doEnemySpecificAction() {
    if (m_tickCount < 10) {
        ++m_tickCount;
        return;
    }
    // reset at new tick
    m_justGotOffLadder = false;
    
    // handle climbing up or down if currently doing so
    if (handleClimbing(true, m_climbingUp) || handleClimbing(false, m_climbingDown)) {
        return;
    }
    
    // check if we've hit a corner (wall or gap ahead)
    bool hitCorner = getWorld()->isBlockedByFloor(getX() + getXMod(), getY()) || 
                    !(getWorld()->isBlockedByFloor(getX() + getXMod(), getY() - 1) || 
                      getWorld()->isOnLadder(getX() + getXMod(), getY() - 1));
                      
    // if we hit a corner
    if (hitCorner) {
        // Try to climb at the corner
        if (tryClimbing()) {
            return;
        }
        // if we can't or won't climb, reverse direction
        setDirection(getDirection() == right ? left : right);
        m_tickCount = 0;
        return;
    } else {
        // Try to climb while walking (not at a corner)
        if (tryClimbing()) {
            return;
        }
    }
    
    // Continue moving in current direction
    moveTo(getX() + getXMod(), getY());
    checkAndHandlePlayerCollision();
    m_tickCount = 0;
}

void Fireball::onAttackBonus() {
    if (randInt(1, 3) == 1) {
        getWorld()->addActor(new GarlicGoodie(getWorld(), getX(), getY()));
    }
}

void Barrel::doEnemySpecificAction() {
    // check if there's no floor beneath
    if (!getWorld()->isBlockedByFloor(getX(), getY() - 1)) {
        moveTo(getX(), getY() - 1);

        // flip direction only once when starting to fall
        if (!m_hasFlipped) {
            setDirection(getDirection() == right ? left : right);
            m_hasFlipped = true;
        }
        return;
    }
    
    // reset flip flag when barrel is on floor
    if (getWorld()->isBlockedByFloor(getX(), getY() - 1)) {
        m_hasFlipped = false;
    }
    
    m_tickCount++;
    if (m_tickCount >= 10) {
        m_tickCount = 0;

        // calculate target position based on current direction
        int targetX = getX() + getXMod();

        // blocked by floor in front, reverse direction
        if (getWorld()->isBlockedByFloor(targetX, getY())) {
            setDirection(getDirection() == right ? left : right);
        } else {
            moveTo(targetX, getY());

            // check if we hit player after moving
            checkAndHandlePlayerCollision();
        }
    }
}

double Kong::getDistanceToPlayer() const {
    int playerX, playerY;
    getWorld()->getPlayerLocation(playerX, playerY);
    
    // Euclidean distance
    double dx = getX() - playerX;
    double dy = getY() - playerY;
    return sqrt(dx * dx + dy * dy);
}

int Kong::getBarrelThrowInterval() const {
    return std::max(200 - 50 * getWorld()->getLevel(), 50);
}

void Kong::doSomething() {
    increaseAnimationNumber();
    
    if (!m_isFleeing && getDistanceToPlayer() <= 2.0) {
        m_isFleeing = true;
    }
    
    if (!m_isFleeing) {
        m_ticksSinceLastBarrel++;
        if (m_ticksSinceLastBarrel >= getBarrelThrowInterval()) {
            m_ticksSinceLastBarrel = 0;
            
            int barrelX = getX() + (2 * getXMod());
            getWorld()->addActor(new Barrel(getWorld(), barrelX, getY(), getDirection()));
        }
    }
    
    m_tickCount++;
    if (m_tickCount >= MOVEMENT_TICK_INTERVAL/2) {  
        m_tickCount = 0;
        
        if (m_isFleeing) {
            // on a ladder or one space above it, keep climbing
            if (getWorld()->isOnLadder(getX(), getY()) || getWorld()->isOnLadder(getX(), getY() + 1)) {
                moveTo(getX(), getY() + 1);
            } else {
                // reached one space above the ladder
                getWorld()->increaseScore(1000);
                getWorld()->playSound(SOUND_FINISHED_LEVEL);
                getWorld()->levelComplete();
                setDead();
            }
        }
    }
}