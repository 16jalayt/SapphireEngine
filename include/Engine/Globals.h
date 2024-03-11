#pragma once
//disable for insecure c functions and a warning about cxx17 standard
#pragma warning( disable : 4996 4038)

//NOTE: brakes FFPlayer
const int globalframerate = 30;
const float frameTime = 1000 / globalframerate;
static int ANIMATION_RATE = 15;

//const float frameTime = 1000 / ANIMATION_RATE;

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

#ifndef SE_API

//COMPILING_DLL must be defined in preprocessor
#if defined(_WIN32)
#if COMPILING_DLL
#define SE_API __declspec(dllexport)
#else
#define SE_API __declspec(dllimport)
#endif
#pragma warning(disable : 4251)

#else
#define SE_API __attribute__((__visibility__("default")))
#endif
//#define LOGURU_EXPORT SE_API;

#endif