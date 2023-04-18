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

#include "AvP_BinkPlayback.h"
#include <assert.h>
#include <chrono>

#include "Nancy/GUI.h"
#include "Engine/utils.h"

static const int kAudioBufferSize = 4096;
static const int kAudioBufferCount = 3;
static const int kQuantum = 1000 / 60;

void* BinkDecodeThread(void* args);
void* BinkAudioThread(void* args);

int BinkPlayback::Open(const std::string& fileName, int x, int y, bool isLooped)
{
	_fileName = fileName;
	_isLooped = isLooped;
	_x = x;
	_y = y;

	// open the file
	_handle = Bink_Open(_fileName.c_str());
	if (!_handle.isValid)
	{
		printf("Can't open Bink file %s\n", _fileName.c_str());
		return 1;
	}

	_frameRate = Bink_GetFrameRate(_handle);

	// get number of audio tracks available
	_nAudioTracks = Bink_GetNumAudioTracks(_handle);

	_audioTrackInfos.resize(_nAudioTracks);

	// init the sound and video api stuff we need
	if (_nAudioTracks)
	{
		// get audio information for all available tracks
		for (uint32_t i = 0; i < _audioTrackInfos.size(); i++) {
			_audioTrackInfos[i] = Bink_GetAudioTrackDetails(_handle, i);
		}

		_audioTrackIndex = 0;

		uint32_t idealBufferSize = _audioTrackInfos[_audioTrackIndex].idealBufferSize;

		// create AudioStream (just for 1 track for now)
		_audioStream = AudioStream_ptr(new AudioStream());
		if (!_audioStream->Init(_audioTrackInfos[_audioTrackIndex].nChannels, _audioTrackInfos[_audioTrackIndex].sampleRate, 16, kAudioBufferSize, kAudioBufferCount))
		{
			printf("Failed to create _audioStream for Bink playback\n");
			return 2;
		}

		// we need some temporary mAudio data storage space and a ring buffer instance
		_audioData = uint8_t_ptr(new uint8_t[idealBufferSize]);
		if (_audioData == NULL)
		{
			printf("Failed to create _audioData stream buffer for Bink playback\n");
			return 2;
		}

		_ringBuffer = RingBuffer_ptr(new RingBuffer());
		if (!_ringBuffer->Init(idealBufferSize * kAudioBufferCount))
		{
			printf("Failed to create _ringBuffer for Bink playback\n");
			return 2;
		}

		_audioDataBuffer = uint8_t_ptr(new uint8_t[idealBufferSize]);
		_audioDataBufferSize = idealBufferSize;
	}

	Bink_GetFrameSize(_handle, _frameWidth, _frameHeight);
	_dims = SDL_Rect{ _x, _y, (int)_frameWidth , (int)_frameHeight };

	_texture = SDL_Texture_ptr(SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, _frameWidth, _frameHeight));
	if (!_texture)
	{
		printf("Unable to create texture(s) for FMV playback\n");
		return 2;
	}

	_fmvPlaying = true;
	_fmvPaused = false;
	_audioStarted = false;
	_frameReady = false;
	_texturesReady = false;

	if (pthread_mutex_init(&_frameCriticalSection, NULL) != 0) {
		return false;
	}
	_frameCriticalSectionInited = true;

	// now start the threads
	if (pthread_create(&_decodeThreadHandle, NULL, BinkDecodeThread, static_cast<void*>(this)) != 0) {
		return false;
	}

	if (_nAudioTracks) {
		if (pthread_create(&_audioThreadHandle, NULL, BinkAudioThread, static_cast<void*>(this)) != 0) {
			return false;
		}
		_audioThreadInited = true;
	}

	_decodeThreadInited = true;

	return 0;
}

BinkPlayback::~BinkPlayback()
{
	this->Close();
}

void BinkPlayback::Close()
{
	if (this)
	{
		_fmvPlaying = false;

		if (_decodeThreadInited) {
			pthread_join(_decodeThreadHandle, NULL);
			//pthread_detach(_decodeThreadHandle);
		}
		if (_audioThreadInited) {
			pthread_join(_audioThreadHandle, NULL);
		}

		if (_frameCriticalSectionInited)
		{
			pthread_mutex_destroy(&_frameCriticalSection);
			_frameCriticalSectionInited = false;
		}
	}
}

bool BinkPlayback::IsPlaying()
{
	return _fmvPlaying;
}

// copies a decoded Theora YUV frame to texture(s) for GPU to convert via shader
bool BinkPlayback::ConvertFrame()
{
	if (!_fmvPlaying) {
		return false;
	}

	// critical section
	pthread_mutex_lock(&_frameCriticalSection);

	SDL_UpdateYUVTexture(
		_texture.get(),
		NULL,
		_yuvBuffer[0].data,
		_yuvBuffer[0].pitch,
		_yuvBuffer[1].data,
		_yuvBuffer[1].pitch,
		_yuvBuffer[2].data,
		_yuvBuffer[2].pitch
	);

	// set this value to true so we can now begin to draw the textured fmv frame
	_texturesReady = true;

	_frameReady = false;

	pthread_mutex_unlock(&_frameCriticalSection);

	return true;
}

void* BinkDecodeThread(void* args)
{
	BinkPlayback* fmv = static_cast<BinkPlayback*>(args);

	while (Bink_GetCurrentFrameNum(fmv->_handle) < Bink_GetNumFrames(fmv->_handle))
	{
		// check if we should still be playing or not
		if (!fmv->IsPlaying()) {
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();

		// critical section
		pthread_mutex_lock(&fmv->_frameCriticalSection);

		Bink_GetNextFrame(fmv->_handle, fmv->_yuvBuffer);

		// we have a new frame and we're ready to use it
		fmv->_frameReady = true;

		pthread_mutex_unlock(&fmv->_frameCriticalSection);

		uint32_t audioSize = Bink_GetAudioData(fmv->_handle, 0, (int16_t*)fmv->_audioDataBuffer.get());

		if (audioSize)
		{
			uint32_t freeSpace = fmv->_ringBuffer->GetWritableSize();

			//			assert(freeSpace >= audioSize);

			if (audioSize > freeSpace)
			{
				while (audioSize > fmv->_ringBuffer->GetWritableSize())
				{
					// little bit of insurance in case we get stuck in here
					if (!fmv->_fmvPlaying) {
						break;
					}

					// wait for the audio buffer to tell us it's just freed up another audio buffer for us to fill
					fmv->_audioStream->WaitForFreeBuffer();
				}
			}

			fmv->_ringBuffer->WriteData((uint8_t*)&fmv->_audioDataBuffer.get()[0], audioSize);
		}

		auto then = std::chrono::high_resolution_clock::now();

		// sleep for frame time minus time to decode frame
		int32_t timeToSleep = (1000 / (int32_t)fmv->_frameRate) - (int32_t)(std::chrono::duration_cast<std::chrono::milliseconds>(then - now).count());
		if (timeToSleep < 0) {
			timeToSleep = 1;
		}

		SDL_Delay(timeToSleep);

		if (fmv->_isLooped)
		{
			// handle looping
			if (Bink_GetCurrentFrameNum(fmv->_handle) >= Bink_GetNumFrames(fmv->_handle)) {
				Bink_GotoFrame(fmv->_handle, 0);
			}
		}
	}

	//We might get to this point because we've played all frames - rather than the game code asking us to stop,
	//or the user interrupting the FMV. Setting _fmvPlaying to false will ensure that the audio decoding thread knows to quit
	fmv->_fmvPlaying = false;
	return 0;
}

//TODO: binkplayback deconstructor not being called

void* BinkAudioThread(void* args)
{
	BinkPlayback_ptr fmv = make_BinkPlayback_s((BinkPlayback*)args);

	int timetoSleep = 0;

	while (fmv->_fmvPlaying)
	{
		auto now = std::chrono::high_resolution_clock::now();

		uint32_t nBuffersFree = fmv->_audioStream->GetNumFreeBuffers();

		while (nBuffersFree)
		{
			uint32_t readableAudio = fmv->_ringBuffer->GetReadableSize();

			// can we fill a buffer
			uint32_t bufferSize = fmv->_audioStream->GetBufferSize();

			if (readableAudio >= bufferSize)
			{
				fmv->_ringBuffer->ReadData(fmv->_audioData.get(), bufferSize);
				fmv->_audioStream->WriteData(fmv->_audioData.get(), bufferSize);
				nBuffersFree--;

				if (fmv->_audioStarted == false)
				{
					fmv->_audioStream->Play();
					fmv->_audioStarted = true;
				}
			}
			else
			{
				// not enough _audioData available this time
				break;
			}
		}

		auto then = std::chrono::high_resolution_clock::now();

		timetoSleep = (int)std::chrono::duration_cast<std::chrono::milliseconds>(then - now).count();
		timetoSleep -= kQuantum;

		if (timetoSleep < 0 || timetoSleep > kQuantum) {
			timetoSleep = 1;
		}
	}

	return 0;
}

SDL_Rect* BinkPlayback::GetDimensions()
{
	return &_dims;
}

void BinkPlayback::SeekFrame(int framenum)
{
	Bink_GotoFrame(_handle, framenum);
}
int BinkPlayback::GetFrameNum()
{
	return Bink_GetCurrentFrameNum(_handle);
}

SDL_Texture_ptr BinkPlayback::GetFrame()
{
	SDL_Texture_ptr texture = SDL_Texture_ptr(SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, _frameWidth, _frameHeight));
	//SDL_Texture_sptr texture = make_SDL_Texture_s(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, _frameWidth, _frameHeight));
	Bink_GetNextFrame(_handle, _yuvBuffer);
	SDL_UpdateYUVTexture(
		texture.get(),
		NULL,
		_yuvBuffer[0].data,
		_yuvBuffer[0].pitch,
		_yuvBuffer[1].data,
		_yuvBuffer[1].pitch,
		_yuvBuffer[2].data,
		_yuvBuffer[2].pitch
	);
	return texture;
}

void BinkPlayback::Draw()
{
	if (_frameReady) {
		ConvertFrame();
	}

	//if (_texturesReady) {
		//SDL_RenderCopy(renderer.get(), _texture.get(), NULL, &_dims);
	//}
	//if (_loaded && _visible)
	//{
	//Never should be part of window
	SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
	//SDL_RenderClear(Graphics::renderer.get());
	SDL_RenderCopy(Graphics::renderer.get(), _texture.get(), NULL, &_dims);
	//Detach the texture
	SDL_SetRenderTarget(Graphics::renderer.get(), NULL);
	//}
}

int	BinkPlayback::OpenPaused(const std::string& fileName, int x, int y, bool isLooped)
{
	Open(fileName, x, y, isLooped);
	_fmvPaused = true;
	return -1;
}

int	BinkPlayback::OpenBackground(const std::string& fileName)
{
	_fileName = fileName;
	_x = 0;
	_y = 0;

	// open the file
	_handle = Bink_Open(_fileName.c_str());
	if (!_handle.isValid)
	{
		printf("Can't open background Bink file %s\n", _fileName.c_str());
		return 1;
	}

	_frameRate = 0;

	Bink_GetFrameSize(_handle, _frameWidth, _frameHeight);
	_dims = SDL_Rect{ _x, _y, (int)_frameWidth , (int)_frameHeight };
	if (GlobalScale != 1)
		setScale(GlobalScale);

	//false so doesn't get next frame
	_frameReady = false;
	//true so draws texure
	_texturesReady = true;

	_texture = GetFrame();
	return 0;
}

void BinkPlayback::setScale(float scale)
{
	_scale = scale;
	//_dims = { (int)(_dims.x * _scale) ,(int)(_dims.y * _scale), (int)(_dims.w * _scale), (int)(_dims.h * _scale) };
	_dims = { _dims.x ,_dims.y, (int)(_dims.w * _scale), (int)(_dims.h * _scale) };
}

float BinkPlayback::getScale()
{
	return _scale;
}