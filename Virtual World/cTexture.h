#pragma once
#include "core.h"
#include "cTGAImg.h"

//wrapper around two helper functions which load TGA files or a Cube Map made of 6 TGA files
class cTexture
{
public:

	static int LoadTGA(char* _TexName, GLuint _TexHandle, GLuint _textureID)
	{
		cTGAImg Img;        // Image loader

		// Load our Texture
		if (Img.Load(_TexName) != IMG_OK)
			return false;

		glBindTexture(GL_TEXTURE_2D, _TexHandle);

		// Create the texture
		if (Img.GetBPP() == 24)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 3, Img.GetWidth(), Img.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
		}
		else if (Img.GetBPP() == 32)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 4, Img.GetWidth(), Img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());
		}
		else
		{
			return 0;
		}

		// Specify filtering and edge actions
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		return Img.GetBPP();
	};

	static int LoadCubeMapTGA(char* _TexName, int _face)
	{
		cTGAImg Img;        // Image loader

		// Load our Texture
		if (Img.Load(_TexName) != IMG_OK)
			return false;


		// Create the texture
		if (Img.GetBPP() == 24)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, 0, GL_RGB, Img.GetWidth(), Img.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
		}
		else if (Img.GetBPP() == 32)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, 0, GL_RGBA, Img.GetWidth(), Img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());
		}
		else
		{
			return 0;
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return Img.GetBPP();
	};
};
