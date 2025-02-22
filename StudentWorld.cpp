#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream> // added for ostringstream
#include <iomanip> // added for setw and setfill
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_level(assetPath)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    // load the current level
    string levelFile = "level00.txt";
    Level::LoadResult result = m_level.loadLevel(levelFile);
    if (result != Level::load_success)
        return GWSTATUS_LEVEL_ERROR;
    
    // iterate through the level grid and create actors
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            Level::MazeEntry item = m_level.getContentsOf(x, y);
            
            switch (item) {
                case Level::player:
                    m_player = new Player(this, x, y);
                    m_actors.push_back(m_player);
                    break;
                case Level::floor:
                    m_actors.push_back(new Floor(this, x, y));
                    break;
                case Level::ladder:
                    m_actors.push_back(new Ladder(this, x, y));
                    break;
                case Level::garlic:
                    m_actors.push_back(new GarlicGoodie(this, x, y));
                    break;
                default:
                    break;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // ask each actor (including player) to do something
    for (Actor* actor : m_actors) {
        if (actor != nullptr && actor->isAlive())
            actor->doSomething();
    }
    
    // Remove dead actors
    vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->isAlive() && *it != m_player) {
            delete *it;
            it = m_actors.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update display text
    setDisplayText();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete all actors
    for (Actor* actor : m_actors) {
        delete actor;
    }
    m_actors.clear();
    m_player = nullptr;  
}

bool StudentWorld::isBlockedByFloor(int x, int y) const {
    // check each actor to see if there's a floor at the specified position
    for (Actor* actor : m_actors) {
        if (actor != nullptr && dynamic_cast<Floor*>(actor) != nullptr) {
            if (actor->getX() == x && actor->getY() == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld::isOnLadder(int x, int y) const {
    // check each actor to see if there's a ladder at the specified position
    for (Actor* actor : m_actors) {
        if (actor != nullptr && dynamic_cast<Ladder*>(actor) != nullptr) {
            if (actor->getX() == x && actor->getY() == y)
                return true;
        }
    }
    return false;
}

void StudentWorld::playSound(int soundId) {
    GameWorld::playSound(soundId);
}

void StudentWorld::setDisplayText()
{
    int score = getScore();  // from GameWorld
    int level = getLevel();  // from GameWorld
    int lives = getLives();  // from GameWorld
    unsigned int burps = m_player ? m_player->getBurpCount() : 0;  // Get burp count from player
    
    // Format: Score: 0000100 Level: 03 Lives: 03 Burps: 08 from spec
    ostringstream oss;
    oss << "Score: " << setw(7) << setfill('0') << score;
    oss << "  Level: " << setw(2) << setfill('0') << level;
    oss << "  Lives: " << setw(2) << setfill('0') << lives;
    oss << "  Burps: " << setw(2) << setfill('0') << burps;
    
    setGameStatText(oss.str());
}

void StudentWorld::attackActorsAt(int x, int y) {
    // Check all actors at the given location
    for (Actor* actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y && actor->canBeAttacked()) {
            actor->attack();
        }
    }
}
