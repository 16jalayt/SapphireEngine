#include "sprite.h"
#include "AnimatedSprite.h"
#include <player-cpp-ffmpeg-sdl/FFPlayer.h>

class Movie : public Sprite
{
public:
	Movie(std::string name, int x, int y, bool isLooped = false, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
	~Movie();
	void Draw();
	//BinkPlayback_ptr FMV;
	FFPlayer_ptr FMV;
	AnimatedSprite_ptr ASprite;

private:
};

using Movie_ptr = std::shared_ptr<Movie>;