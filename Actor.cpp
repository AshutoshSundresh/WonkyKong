#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Player::doSomething() {
    int ch;
    if (getWorld()->getKey(ch)) { 
        // get the target coordinates based on the key press
        int targetX = getX();
        int targetY = getY();
        
        switch (ch) {
            case KEY_PRESS_LEFT:  
                setDirection(180);  
                targetX = getX() - 1;  // try to move one unit left
                break;
            case KEY_PRESS_RIGHT:  
                setDirection(0);  
                targetX = getX() + 1;  // y to move one unit right
                break;
        }
        
        // only move if the target position is not blocked by a floor
        if (!getWorld()->isBlockedByFloor(targetX, targetY))
            moveTo(targetX, targetY);
    }
}
