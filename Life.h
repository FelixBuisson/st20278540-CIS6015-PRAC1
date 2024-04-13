#pragma once
#include "IComputeShader.h"

// Conway's Game of Life shader

class Life :public IComputeShader
{
	GLuint			m_data;
	int current = 0;

public:

	Life() { m_size = 1024; };

	void GenerateTextures(GLuint _w, GLuint _h);

	void SimBody();

	GLuint GetTexture() { return m_textures[current]; }
};