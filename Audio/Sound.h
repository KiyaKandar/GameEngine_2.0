#pragma once

#include "OpenAL 1.1 SDK\include\al.h"
#include "..\Resource Management\Resources\Resource.h"

#include <string>

using namespace std;

typedef unsigned long DWORD;

struct FMTCHUNK 
{
	short format;
	short channels;
	DWORD srate;
	DWORD bps;
	short balign;
	short samp;
};

class Sound : public Resource
{
public:

	Sound(std::string filePath);
	~Sound();

	char* GetData() const
	{
		return data;
	}

	int GetBitRate() const
	{
		return bitRate;
	}

	float GetFrequency() const
	{
		return freqRate;
	}

	int GetChannels() const
	{
		return channels;
	}

	int GetSize() const
	{
		return size;
	}

	ALuint GetBuffer() const
	{
		return buffer;
	}

	std::string getSoundFile() const
	{
		return file;
	}

	ALenum GetOalFormat();
	double GetLength();

	void LoadFromWav(string filename);
	void LoadWavChunkInfo(ifstream &file, string &name, unsigned int &size);

private:
	ALuint buffer;
	std::string file;

	char *data;
	float freqRate;
	double length;

	unsigned int bitRate;
	unsigned int size;
	unsigned int channels;
};