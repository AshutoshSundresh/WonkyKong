#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Player::doSomething() {
    int ch;
    if (getWorld()->getKey(ch)) { 
        switch (ch) {
            case KEY_PRESS_LEFT:  
                setDirection(180);  
                moveTo(getX() - 1, getY());  
                break;
            case KEY_PRESS_RIGHT:  
                setDirection(0);  
                moveTo(getX() + 1, getY());  
                break;
        }
    }
}
