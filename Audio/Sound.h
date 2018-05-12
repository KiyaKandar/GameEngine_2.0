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

	char* getData() const
	{
		return data;
	}

	int getBitRate() const
	{
		return bitRate;
	}

	float getFrequency() const
	{
		return freqRate;
	}

	int getChannels() const
	{
		return channels;
	}

	int getSize() const
	{
		return size;
	}

	ALuint getBuffer() const
	{
		return buffer;
	}

	std::string getSoundFile()
	{
		return file;
	}

	ALenum getOALFormat();
	double getLength();

	void loadFromWAV(string filename);
	void loadWAVChunkInfo(ifstream &file, string &name, unsigned int &size);

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