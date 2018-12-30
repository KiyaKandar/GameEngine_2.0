#pragma once
#include <vector>
#include "Mesh.h"
#include "../NCLGL Legacy/LegacyMesh.h"

/*
A TextMesh uses a 'Font', simply a struct which holds a texture, and some info
about that texture. bitmap fonts have a grid like array of cells, usually
arranged so that they are in ascending byte order. So by storing the
number of cells across and down the font texture has, we can work out which
bit of the texture is which text character (assuming they are in byte order).
The Font class cleans up after itself, so we don't have to keep track of its
font texture anywhere else!
*/
struct Font
{
	GLuint texture;
	int xCount;
	int yCount;

	Font(GLuint tex, unsigned int xCount, unsigned int yCount)
	{
		this->texture = tex;
		this->xCount = xCount;
		this->yCount = yCount;
	}

	~Font()
	{
		//glDeleteTextures(1, &texture);
	}
};

class TextMesh : public LegacyMesh
{
public:
	TextMesh(const std::string& text, const Font font);
	~TextMesh(void);

protected:

	const Font& font;
};
