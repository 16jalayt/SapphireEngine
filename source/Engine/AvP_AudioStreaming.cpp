#include "AvP_audioStreaming.h"
#include <assert.h>
//#include <math.h>
//#include <new>
//#include "FileStream.h"
#include <AL\al.h>
#include <AL\alc.h>
#include <AL\alext.h>
#include <AL\efx-presets.h>
//#include <Windows.h>
#include <vector>
#include <SDL2/SDL_timer.h>

static bool		soundEnabled = false;
static ALCdevice* device = 0;
static ALCcontext* context = 0;
std::vector<sndSource> sourceList;

ALenum AlCheckError()
{
	ALenum error = alGetError();
	switch (error)
	{
	case AL_NO_ERROR:
		// fine.
		break;
	case AL_INVALID_NAME:
		printf("OpenAL Error - AL_INVALID_NAME\n");
		break;
	case AL_INVALID_ENUM:
		printf("OpenAL Error - AL_INVALID_ENUM\n");
		break;
	case AL_INVALID_OPERATION:
		printf("OpenAL Error - AL_INVALID_OPERATION\n");
		break;
	case AL_OUT_OF_MEMORY:
		printf("OpenAL Error - AL_OUT_OF_MEMORY\n");
		break;
	}
	return error;
}
static const float vol_to_gain_table[] = {
	0.000010f, 0.000011f, 0.000056f, 0.000146f, 0.000287f, 0.000486f, 0.000748f, 0.001076f,
	0.001474f, 0.001945f, 0.002495f, 0.003122f, 0.003837f, 0.004629f, 0.005514f, 0.006494f,
	0.007560f, 0.008720f, 0.009977f, 0.011337f, 0.012794f, 0.014355f, 0.016014f, 0.017783f,
	0.019656f, 0.021652f, 0.023741f, 0.025942f, 0.028281f, 0.030726f, 0.033266f, 0.035934f,
	0.038726f, 0.041639f, 0.044668f, 0.047863f, 0.051168f, 0.054576f, 0.058076f, 0.061731f,
	0.065539f, 0.069502f, 0.073536f, 0.077714f, 0.082130f, 0.086596f, 0.091201f, 0.095940f,
	0.100809f, 0.105803f, 0.110917f, 0.116279f, 0.121759f, 0.127350f, 0.133045f, 0.138995f,
	0.144877f, 0.151182f, 0.157398f, 0.163870f, 0.170608f, 0.177419f, 0.184289f, 0.191426f,
	0.198609f, 0.206063f, 0.213550f, 0.221309f, 0.229087f, 0.237137f, 0.245188f, 0.253805f,
	0.262120f, 0.270707f, 0.279576f, 0.288735f, 0.297852f, 0.307256f, 0.316592f, 0.326212f,
	0.336124f, 0.345939f, 0.356451f, 0.366438f, 0.377138f, 0.387704f, 0.398566f, 0.409732f,
	0.420727f, 0.432016f, 0.443609f, 0.455512f, 0.467197f, 0.479181f, 0.491473f, 0.504081f,
	0.516416f, 0.529663f, 0.542625f, 0.555265f, 0.568853f, 0.582103f, 0.596348f, 0.610239f,
	0.623735f, 0.638263f, 0.652379f, 0.667575f, 0.682339f, 0.696626f, 0.712033f, 0.727780f,
	0.743019f, 0.758578f, 0.774462f, 0.790679f, 0.807235f, 0.824138f, 0.840427f, 0.857038f,
	0.873977f, 0.891251f, 0.908866f, 0.926830f, 0.945148f, 0.962720f, 0.980618f, 1.000000f
};

//TODO: gets called before AudioStreams are released. Threads still running.
//see if gets fixed when fmvs moved out of main
//UPDATE: nope
void PlatEndSoundSys()
{
	// lets check all sounds are stopped and released here (important to handle alt+f4 close)
	//SoundSys_RemoveAll();

	// release sources
	for (size_t i = 0; i < sourceList.size(); i++) {
		alSourceStop(sourceList[i]._alSource);
		alSourcei(sourceList[i]._alSource, AL_BUFFER, NULL);
		alDeleteSources(1, &sourceList[i]._alSource);
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

#ifdef _DEBUG
	//printf("Uninitialised OpenAL successfully\n");
#endif
}

int PlatStartSoundSys()
{
#ifdef _DEBUG
	char buf[256];
	//printf("Starting to initialise OpenAL\n");
#endif

	uint32_t sampleRate = 44100;
	char* DefaultDevice = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

#ifdef _DEBUG
	sprintf(buf, "Using default device \"%s\" for OpenAL", DefaultDevice);
	//printf(buf);
#endif

	device = alcOpenDevice(NULL);
	if (device == NULL) {
		printf("alcOpenDevice failed for OpenAL\n");
		PlatEndSoundSys();
		return 0;
	}

	context = alcCreateContext(device, NULL);
	if (context == NULL) {
		printf("alcCreateContext failed for OpenAL\n");
		PlatEndSoundSys();
		return 0;
	}

	//alGetError(); // clear error stack

	alcMakeContextCurrent(context);
	AlCheckError();

	alGetError(); // clear error stack

	// try create up to 255 sources
	for (size_t i = 0; i < 255; i++)
	{
		sndSource newSource;
		alGenSources(1, &newSource._alSource);

		if (alGetError() != AL_NO_ERROR) {
			break;
		}

		// set default properties
		alSourcef(newSource._alSource, AL_PITCH, 1.0f);
		alSourcef(newSource._alSource, AL_GAIN, 1.0f);
		alSourcef(newSource._alSource, AL_ROLLOFF_FACTOR, 0.01f);
		alSourcef(newSource._alSource, AL_REFERENCE_DISTANCE, 1.0f);
		alSource3f(newSource._alSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(newSource._alSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		alSourcei(newSource._alSource, AL_LOOPING, AL_FALSE);

		sourceList.push_back(newSource);
		alGetError(); // clear error stack
	}

	if (sourceList.size() == 0) { // should we allow less than 255? 32 sources?
		printf("Source creation failed for OpenAL\n");
		PlatEndSoundSys();
		return 0;
	}
#ifdef _DEBUG
	else
	{
		sprintf(buf, "Created %zu sources for OpenAL\n", sourceList.size());
		//printf(buf);
	}
#endif

	alGetError(); // clear error stack

	alDistanceModel(AL_NONE);

	AlCheckError();

#ifdef _DEBUG
	//printf("Initialised OpenAL successfully\n");
#endif

	soundEnabled = true;

	return 1;
}

inline ALenum GetALFormat(uint16_t bitsPerSample, uint16_t nChannels)
{
	ALenum alFormat = 0;

	if (bitsPerSample == 8)
	{
		if (nChannels == 1) {
			alFormat = AL_FORMAT_MONO8;
		}
		else if (nChannels == 2) {
			alFormat = AL_FORMAT_STEREO8;
		}
	}
	else if (bitsPerSample == 16)
	{
		if (nChannels == 1) {
			alFormat = AL_FORMAT_MONO16;
		}
		else if (nChannels == 2) {
			alFormat = AL_FORMAT_STEREO16;
		}
	}
	else {
		assert(1 == 0);
	}
	return alFormat;
}

AudioStream::AudioStream()
{
	_source = NULL;
	_bufferSize = 0;
	_bufferCount = 0;
	_currentBuffer = 0;
	_nChannels = 0;
	_rate = 0;
	_bytesPerSample = 0;
	_totalBytesPlayed = 0;
	_totalSamplesWritten = 0;
	_isPaused = false;
	_isPlaying = false;
	_buffers = NULL;
	_dataBuffers = NULL;
	_format = 0;
	_volume = 0;
}

static sndSource* FindFreeSource()
{
	for (size_t i = 0; i < sourceList.size(); i++)
	{
		if (sourceList[i]._isFree)
		{
			// set some default values
			alSourcef(sourceList[i]._alSource, AL_PITCH, 1.0f);
			alSourcef(sourceList[i]._alSource, AL_GAIN, 1.0f);
			alSourcei(sourceList[i]._alSource, AL_LOOPING, AL_FALSE);
			alSource3f(sourceList[i]._alSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
			alSource3f(sourceList[i]._alSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
			alSource3f(sourceList[i]._alSource, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
			alSourcef(sourceList[i]._alSource, AL_ROLLOFF_FACTOR, 0.0f);
			alSourcei(sourceList[i]._alSource, AL_SOURCE_RELATIVE, AL_TRUE);

			sourceList[i]._isFree = false;
			alGetError(); // clear error stack

			return &sourceList[i];
		}
	}
	return NULL;
}

bool AudioStream::Init(uint32_t nChannels, uint32_t rate, uint32_t bitsPerSample, uint32_t bufferSize, uint32_t nBuffers)
{
	if (!soundEnabled) {
		return false;
	}

	_format = GetALFormat(bitsPerSample, nChannels);

	// allocate space for n AL buffers
	_buffers = new(std::nothrow) ALuint[nBuffers];
	if (NULL == _buffers) {
		printf("Out of memory trying to create streaming audio AL buffer(s)\n");
		return false;
	}

	// allocate space where actual audio data goes
	_dataBuffers = new(std::nothrow) uint8_t[bufferSize * nBuffers];
	if (NULL == _dataBuffers) {
		printf("Out of memory trying to create streaming audio data buffer\n");
		return false;
	}

	// clear OpenAL errors
	alGetError();

	alGenBuffers(nBuffers, _buffers);
	AlCheckError();

	_source = FindFreeSource();
	if (!_source) {
		printf("No free sources available for streaming audio\n");
		return false;
	}
	_bytesPerSample = bitsPerSample / 8;
	_nChannels = nChannels;
	_rate = rate;
	_bufferSize = bufferSize;
	_bufferCount = nBuffers;

	// set this to true for threads to run
	_isPlaying = true;

	for (uint32_t i = 0; i < nBuffers; i++) {
		_freeBufferQueue.push_back(_buffers[i]);
	}

	if (pthread_create(&_playbackThread, NULL, CheckProcessedBuffers, static_cast<void*>(this)) != 0) {
		return false;
	}
	if (pthread_mutex_init(&_bufferMutex, NULL) != 0) {
		return false;
	}

	return true;
}

uint32_t AudioStream::WriteData(uint8_t* audioData, uint32_t size)
{
	assert(audioData);
	assert(size == _bufferSize);

	// clear OpenAL errors
	alGetError();

	if (_freeBufferQueue.size()) {
		pthread_mutex_lock(&_bufferMutex);

		ALuint buffer = _freeBufferQueue.front();
		_freeBufferQueue.pop_front();

		memcpy(&_dataBuffers[_currentBuffer * _bufferSize], audioData, size);

		alBufferData(buffer, _format, &_dataBuffers[_currentBuffer * _bufferSize], size, _rate);
		AlCheckError();

		// queue it
		alSourceQueueBuffers(_source->_alSource, 1, &buffer);
		AlCheckError();

		pthread_mutex_unlock(&_bufferMutex);
	}

	_currentBuffer++;
	_currentBuffer %= _bufferCount;

	// size in bytes divided by bytes per sample divided by the number of channels
	_totalSamplesWritten += ((size / _bytesPerSample) / _nChannels);

	return size;
}

void AudioStream::WaitForFreeBuffer()
{
	while (_freeBufferQueue.size() == 0) {
		SDL_Delay(20);
	}
}

void* CheckProcessedBuffers(void* args)
{
	AudioStream* stream = static_cast<AudioStream*>(args);

	while (stream->_isPlaying)
	{
		if (!stream->_isPaused)
		{
			// clear OpenAL errors
			alGetError();

			ALint nProcessed;
			alGetSourcei(stream->_source->_alSource, AL_BUFFERS_PROCESSED, &nProcessed);
			AlCheckError();

			if (nProcessed && (stream->_freeBufferQueue.size() < stream->_bufferCount))
			{
				pthread_mutex_lock(&stream->_bufferMutex);

				ALuint buffer;
				alSourceUnqueueBuffers(stream->_source->_alSource, 1, &buffer);
				AlCheckError();

				stream->_freeBufferQueue.push_back(buffer);
				stream->_totalBytesPlayed += stream->_bufferSize;

				pthread_mutex_unlock(&stream->_bufferMutex);
			}
			else {
				// wait and try again
				SDL_Delay(1000 / 60);
			}
		}
	}

	return 0;
}

uint32_t AudioStream::GetBufferSize()
{
	return _bufferSize;
}

uint32_t AudioStream::GetNumFreeBuffers()
{
	return (uint32_t)_freeBufferQueue.size();
}

uint64_t AudioStream::GetNumSamplesPlayed()
{
	return ((_totalBytesPlayed / _bytesPerSample) / _nChannels);
}

uint64_t AudioStream::GetNumSamplesWritten()
{
	return _totalSamplesWritten;
}

uint32_t AudioStream::GetWritableBufferSize()
{
	return _bufferSize * (uint32_t)_freeBufferQueue.size();
}

int32_t AudioStream::SetVolume(uint32_t volume)
{
	alSourcef(_source->_alSource, AL_GAIN, vol_to_gain_table[volume]);
	alGetError();

	return 1;
}

uint32_t AudioStream::GetVolume()
{
	return _volume;
}

int32_t AudioStream::SetPan(uint32_t pan)
{
	// todo: implement this function
	return AUDIOSTREAM_OK;
}

int32_t AudioStream::Stop()
{
	_isPlaying = false;

	alSourceStop(_source->_alSource);
	alGetError();

	return AUDIOSTREAM_OK;
}

int32_t AudioStream::Play()
{
	alSourcePlay(_source->_alSource);
	AlCheckError();
	_isPaused = false;

	return AUDIOSTREAM_OK;
}

AudioStream::~AudioStream()
{
	Stop();

	pthread_join(_playbackThread, NULL);
	pthread_mutex_destroy(&_bufferMutex);

	alSourcei(_source->_alSource, AL_BUFFER, NULL);
	alDeleteSources(1, &_source->_alSource);

	_source->_isFree = true;
	_source = NULL;

	alDeleteBuffers(_bufferCount, _buffers);

	// clear the new-ed memory
	//crashes for some reason

	if (_buffers != nullptr)
	{
		delete[] _buffers;
		_buffers = NULL;
	}
	if (_dataBuffers != nullptr)
	{
		delete[] _dataBuffers;
		_dataBuffers = NULL;
	}
}