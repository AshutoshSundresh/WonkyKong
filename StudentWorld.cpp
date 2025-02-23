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
: GameWorld(assetPath), m_player(nullptr), m_level(assetPath), m_levelComplete(false)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    m_levelComplete = false;

    // load the current level
    string levelFile = "level";
    levelFile += (getLevel() < 10 ? "0" : "") + to_string(getLevel()) + ".txt";
    Level::LoadResult result = m_level.loadLevel(levelFile);
    if (result != Level::load_success)
        return GWSTATUS_LEVEL_ERROR;
    
    // create actors
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
                case Level::extra_life:
                    m_actors.push_back(new ExtraLifeGoodie(this, x, y));
                    break;
                case Level::bonfire:
                    m_actors.push_back(new Bonfire(this, x, y));
                    break;
                case Level::koopa:
                    m_actors.push_back(new Koopa(this, x, y));
                    break;
                case Level::fireball:
                    m_actors.push_back(new Fireball(this, x, y));
                    break;  
                case Level::right_kong:
                    m_actors.push_back(new Kong(this, x, y, 1));  // 1 for right-facing
                    break;  
                case Level::left_kong:
                    m_actors.push_back(new Kong(this, x, y, 2));  // != 1 for left-facing
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
    setDisplayText();
    
    for (Actor* actor : m_actors) {
        if (actor != nullptr && actor->isAlive()) {
            actor->doSomething();
            
            // if player died during this actor's action
            if (!m_player->isAlive())
                return GWSTATUS_PLAYER_DIED;
                
            // if kong fled
            if (m_levelComplete)
                return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->isAlive() && *it != m_player) {
            delete *it;
            it = m_actors.erase(it);
        } else {
            ++it;
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_actors.size() == 0 && m_player == nullptr) {
        return;
    }

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

bool StudentWorld::isBonfireAt(int x, int y) const {
    for (Actor* actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y) {
            Bonfire* bonfire = dynamic_cast<Bonfire*>(actor);
            if (bonfire != nullptr)
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
    // Format: Score: 0000100 Level: 03 Lives: 03 Burps: 08 from spec
    ostringstream oss;
    oss << "Score: " << setw(7) << setfill('0') << getScore()
        << "  Level: " << setw(2) << getLevel()
        << "  Lives: " << setw(2) << getLives()
        << "  Burps: " << setw(2) << (m_player ? m_player->getBurpCount() : 0);

    setGameStatText(oss.str());
}

void StudentWorld::attackNonPlayerActorsAt(int x, int y) {
    // attack all actors at the given location, excluding the player
    for (Actor* actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y && actor->canBeAttacked() && actor != m_player) {
            actor->attack();
        }
    }
}
