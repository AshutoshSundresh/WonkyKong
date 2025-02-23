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
            targetX += (getDirection() == right ? 1 : -1);
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
    int burpX = getX() + (getDirection() == right ? 1 : -1);
    int burpY = getY();
    
    // play burp sound
    getWorld()->playSound(SOUND_BURP);
    
    // create and add burp to the world
    getWorld()->addActor(new Burp(getWorld(), burpX, burpY, getDirection()));
    
    // decrease burp count
    decrementBurpCount();
}

void Player::attack() {
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
    
    // attack any attackable actor in the current square
    getWorld()->attackActorsAt(getX(), getY());
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
    doEnemySpecificAction();
    
    // Check for collision with player and handle death
    if (getWorld()->isPlayerAt(getX(), getY())) {
        getWorld()->decLives();  
        getWorld()->playSound(SOUND_PLAYER_DIE);  
        getWorld()->getPlayer()->setDead();  
        return; 
    }
}

void Enemy::attack() {
    if (!isAlive())
        return;
        
    setDead();
    getWorld()->playSound(SOUND_ENEMY_DIE);
    getWorld()->increaseScore(100);
    
    onAttackBonus();
}

void Bonfire::doEnemySpecificAction() {
    increaseAnimationNumber();  // animate the flames
}

// koopa can die unlike the base enemy doSomething, it also does not kill the player
void Koopa::doSomething() {
    if (!isAlive())
        return;
        
    // Check if on same square as player and can freeze
    if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
        getWorld()->freezePlayer();
        m_freezeCooldown = 50;  // Set cooldown timer
        return;
    }

    // Decrement freeze cooldown if active
    if (m_freezeCooldown > 0)
        m_freezeCooldown--;

    // Handle movement every 10 ticks
    m_moveCounter = (m_moveCounter + 1) % 10;
    if (m_moveCounter == 0) {
        doEnemySpecificAction();
    }
}

void Koopa::doEnemySpecificAction() {
    // handle Koopa's movement here
    // Determine target position based on direction
    int targetX = getX() + (getDirection() == right ? 1 : -1);
    
    // check if movement is valid
    bool hasFloorAhead = getWorld()->isBlockedByFloor(targetX, getY());
    bool hasFloorBelow = getWorld()->isBlockedByFloor(targetX, getY() - 1);
    bool hasLadderBelow = getWorld()->isOnLadder(targetX, getY() - 1);
    
    // Can move if there's no floor ahead AND (there's floor OR ladder below)
    if (!hasFloorAhead && (hasFloorBelow || hasLadderBelow)) {
        moveTo(targetX, getY());
        
        if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
            getWorld()->freezePlayer();
            m_freezeCooldown = 50;
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

void Fireball::doEnemySpecificAction() {
    if (!isAlive())
        return;

    // Only perform movement every 10 ticks
    m_tickCount++;
    if (m_tickCount % 10 != 0)
        return;
        
    // Check for ladder climbing opportunities
    if (getWorld()->isOnLadder(getX(), getY())) {
        // Check for climbing up
        if (!m_isClimbing || (m_isClimbing && m_climbingUp)) {
            if (!getWorld()->isBlockedByFloor(getX(), getY() + 1)) {
                if (m_climbingUp || randInt(1, 3) == 1) {
                    m_isClimbing = true;
                    m_climbingUp = true;
                    moveTo(getX(), getY() + 1);
                    
                    // Check for player collision after moving
                    if (getWorld()->isPlayerAt(getX(), getY())) {
                        getWorld()->getPlayer()->attack();
                    }
                    return;
                }
            } else {
                m_isClimbing = false;  // Hit ceiling, stop climbing
            }
        }
    }
    
    // Check for climbing down
    if (getWorld()->isOnLadder(getX(), getY() - 1)) {
        if (!m_isClimbing || (m_isClimbing && !m_climbingUp)) {
            if (!m_climbingUp || randInt(1, 3) == 1) {
                m_isClimbing = true;
                m_climbingUp = false;
                moveTo(getX(), getY() - 1);
                
                // Check for player collision after moving
                if (getWorld()->isPlayerAt(getX(), getY())) {
                    getWorld()->getPlayer()->attack();
                }
                return;
            }
        }
    } else if (m_isClimbing && !m_climbingUp) {
        m_isClimbing = false;  // No more ladder below, stop climbing
    }
    
    // If not climbing, handle horizontal movement
    if (!m_isClimbing) {
        int nextX = getX() + (getDirection() == right ? 1 : -1);
        
        // Check if next position is valid
        if (getWorld()->isBlockedByFloor(nextX, getY()) || 
            (!getWorld()->isBlockedByFloor(nextX, getY() - 1) && 
             !getWorld()->isOnLadder(nextX, getY() - 1))) {
            // Reverse direction if blocked or would fall
            setDirection(getDirection() == right ? left : right);
        } else {
            // Move in current direction
            moveTo(nextX, getY());
            
            // Check for player collision after moving
            if (getWorld()->isPlayerAt(getX(), getY())) {
                getWorld()->getPlayer()->attack();
            }
        }
    }
}

void Fireball::onAttackBonus() {
    if (randInt(1, 3) == 1) {
        getWorld()->addActor(new GarlicGoodie(getWorld(), getX(), getY()));
    }
}