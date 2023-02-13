// Copyright (C) 2011 Barry Duncan. All Rights Reserved.
// The original author of this code can be contacted at: bduncan22@hotmail.com

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// DEVELOPERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _BinkPlayback_h_
#define _BinkPlayback_h_

#include "libbinkdec/BinkDecoder.h"
#include "AvP_AudioStreaming.h"
#include "AvP_RingBuffer.h"
#include <pthread.h>
#include <SDL2/SDL.h>
#include "graphics.h"
#include <memory.h>

using AudioStream_ptr = std::shared_ptr<AudioStream>;
using RingBuffer_ptr = std::shared_ptr<RingBuffer>;
using uint8_t_ptr = std::shared_ptr<uint8_t>;

class BinkPlayback
{
private:
	// thread handles
	pthread_t _decodeThreadHandle;
	pthread_t _audioThreadHandle;
	bool _decodeThreadInited;
	bool _audioThreadInited;
	bool _frameCriticalSectionInited;

public:
	//TODO: change assesability and use getters
	BinkHandle _handle;
	std::string _fileName;
	// audio
	uint32_t _nAudioTracks;
	std::vector<AudioInfo> _audioTrackInfos;

	uint32_t _audioTrackIndex;

	AudioStream_ptr _audioStream;
	RingBuffer_ptr _ringBuffer;

	uint8_t_ptr _audioData;
	uint8_t_ptr _audioDataBuffer;
	uint32_t	  _audioDataBufferSize;

	// video
	YUVbuffer _yuvBuffer;

	SDL_Texture_ptr _texture;

	uint32_t _frameWidth;
	uint32_t _frameHeight;
	int _x;
	int _y;
	float _scale;
	SDL_Rect _dims = { 0, 0, 0, 0 };

	pthread_mutex_t _frameCriticalSection;

	volatile bool _fmvPlaying;
	volatile bool _frameReady;
	bool _audioStarted;
	volatile bool _texturesReady;
	bool _isLooped;
	float _frameRate;

	BinkPlayback() :
		_decodeThreadInited(false),
		_audioThreadInited(false),
		_frameCriticalSectionInited(false),
		_nAudioTracks(0),
		_audioDataBuffer(0),
		_audioDataBufferSize(0),
		_audioTrackIndex(0),
		_frameWidth(0),
		_frameHeight(0),
		_x(0),
		_y(0),
		_fmvPlaying(false),
		_frameReady(false),
		_audioStarted(false),
		_texturesReady(false),
		_isLooped(false),
		_frameRate(0),
		_scale(1)
	{
	}
	~BinkPlayback();

	// functions
	int	Open(const std::string& fileName, int x, int y, bool isLooped = false);
	void Close();
	bool IsPlaying();
	bool ConvertFrame();
	void SeekFrame(int framenum);
	int GetFrameNum();
	SDL_Texture_ptr GetFrame();
	SDL_Rect* GetDimensions();
	void Draw();
	//draw. sould be complete object
	int	OpenPaused(const std::string& fileName, int x, int y, bool isLooped = false);
	int	OpenBackground(const std::string& fileName);
	void setScale(float scale);
	float getScale();
};

struct BinkPlayback_Deleter
{
	void operator()(BinkPlayback* fmv)
	{
		fmv->Close();
	}

	/*static void close(BinkPlayback* fp) {
		if (fp) {
		}
	}*/
};

/*void destroy(BinkPlayback* fmv)
{
	fmv->Close();
}*/

//auto l = [](BinkPlayback* p) { p->Close(); };
//using BinkPlayback_ptr = std::unique_ptr<BinkPlayback, decltype(l) >;
//using BinkPlayback_ptr = std::unique_ptr<BinkPlayback, BinkPlayback_Deleter>;
using BinkPlayback_ptr = std::shared_ptr<BinkPlayback>;
template <class... Args>
std::shared_ptr<BinkPlayback> make_BinkPlayback_s(Args&&... args)
{
	return std::shared_ptr<BinkPlayback>(std::forward<Args>(args)...,
		BinkPlayback_Deleter{});
}

#endif