#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// image IDs for the game objects

const int IID_PLAYER = 0;
const int IID_KONG = 1;
const int IID_BARREL = 2;
const int IID_FIREBALL = 3;
const int IID_KOOPA = 4;
const int IID_FLOOR = 5;
const int IID_LADDER = 6;
const int IID_EXTRA_LIFE_GOODIE = 7;
const int IID_GARLIC_GOODIE = 8;
const int IID_BONFIRE = 9;
const int IID_BURP = 10;

// sounds

const int SOUND_THEME = 0;
const int SOUND_ENEMY_DIE = 1;
const int SOUND_PLAYER_DIE = 2;
const int SOUND_BURP = 3;
const int SOUND_GOT_GOODIE = 4;
const int SOUND_FINISHED_LEVEL = 5;
const int SOUND_JUMP = 6;

const int SOUND_NONE = -1;

// keys the user can hit

const int KEY_PRESS_LEFT = 1000;
const int KEY_PRESS_RIGHT = 1001;
const int KEY_PRESS_UP = 1002;
const int KEY_PRESS_DOWN = 1003;
const int KEY_PRESS_TAB = '\t';
const int KEY_PRESS_SPACE = ' ';
//const int KEY_PRESS_ENTER = '\r';
//const int KEY_PRESS_ESCAPE = '\x1b';

// board dimensions

const int VIEW_WIDTH = 20;
const int VIEW_HEIGHT = 20;

// game status

const int GWSTATUS_CONTINUE_GAME = 0;
const int GWSTATUS_FINISHED_LEVEL = 1;
const int GWSTATUS_PLAYER_WON = 2;
const int GWSTATUS_PLAYER_DIED = 3;
const int GWSTATUS_LEVEL_ERROR = 4;
const int GWSTATUS_NOT_IMPLEMENTED = 5;

// other constants

const int MAX_LEVELS = 99;

// display constants

const double SPRITE_WIDTH_GL = .48; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .4; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
	if (max < min)
		std::swap(max, min);
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	std::uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

#endif // GAMECONSTANTS_H_
