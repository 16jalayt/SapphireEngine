#pragma once

const bool oldUI = true;
const bool debugHot = true;
const bool fullscreen = false;
//TODO: bik player does not respect
const bool debugNoSound = true;

//logical canvas resolution
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int ANIMATION_RATE = 15;

#ifdef __SWITCH__
const int REAL_WIDTH = 1280;
const int REAL_HEIGHT = 720;
#else
const int REAL_WIDTH = 800;
const int REAL_HEIGHT = 600;
#endif

//working directory: $(SolutionDir)$(PlatformShortName)\$(Configuration)\

#ifdef __SWITCH__
enum pad_mapping
{
	KEY_A, KEY_B, KEY_X, KEY_Y, // 0, 1, 2, 3
	KEY_LSTICK, KEY_RSTICK, // 4, 5
	KEY_L, KEY_R, // 6, 7
	KEY_ZL, KEY_ZR, // 8, 9
	KEY_PLUS, KEY_MINUS, // 10, 11
	KEY_DLEFT, KEY_DUP, KEY_DRIGHT, KEY_DDOWN, // 12, 13, 14, 15
	KEY_LSTICK_LEFT, KEY_LSTICK_UP, KEY_LSTICK_RIGHT, KEY_LSTICK_DOWN, // 16, 17, 18, 19
	KEY_RSTICK_LEFT, KEY_RSTICK_UP, KEY_RSTICK_RIGHT, KEY_RSTICK_DOWN, // 20, 21, 22, 23
	KEY_SL_LEFT, KEY_SR_LEFT, KEY_SL_RIGHT, KEY_SR_RIGHT // 24, 25, 26, 28
};
#endif

typedef struct Scaled_Rect
{
	int x, y;
	int x2, y2;
	//Scaled_Rect() : x(0), y(0), x2(0), y2(0) { }
	//Scaled_Rect(int 2x=0, int 2y=0, int 2x2=0, int 2y2=0) : x(2x), y(2y), x2(2x2), y2(2y2) { }
	/*Scaled_Rect(int x, int y, int x2, int y2) {
		Scaled_Rect sr;
		sr.x = x;
		sr.y = y;
		sr.x2 = x2;
		sr.y2 = y2;
	}*/
} Scaled_Rect;

/*typedef struct Scaled_Rect
{
	int x, y;
	int x2, y2;
} Scaled_Rect;*/

inline bool operator==(const Scaled_Rect& a, const Scaled_Rect& b)
{
	return a.x == b.x && a.y == b.y && a.x2 == b.x2 && a.y2 == b.y2;
}

inline bool operator!=(const Scaled_Rect& a, const Scaled_Rect& b)
{
	return !(a.x == b.x && a.y == b.y && a.x2 == b.x2 && a.y2 == b.y2);
}

/*inline bool operator==(const SDL_Rect& a, const SDL_Rect& b)
{
	return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}*/

typedef Scaled_Rect UnScaled_Rect;

//const int BackgroundOffsetX = 52;//0 for newer nd
//const int BackgroundOffsetY = 18;//55 for newer nd
const float GlobalScale = 1.25;//1.25 for newer nd
//const int UpperUIOffset = 0;//0 for newer nd
//const int LowerUIOffset = 320;//536 for newer nd