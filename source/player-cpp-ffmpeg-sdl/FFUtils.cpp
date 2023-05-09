#include "defs.h"
#include "Player.h"
#include "FFUtils.h"

void FFUtils::display_exception(const char* message)
{
	Player::get_instance()->clear();
	throw std::runtime_error(message);
}

void FFUtils::display_ffmpeg_exception(int error_code)
{
	char error_buf[ERROR_SIZE];
	av_strerror(error_code, error_buf, ERROR_SIZE);
	display_exception(error_buf);
}