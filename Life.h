#pragma once
#include "IComputeShader.h"

// Conway's Game of life shader

class life :public IComputeShader
{
	GLuint			m_data;
	int current = 0;

public:

	life() { m_size = 1024; };

	void GenerateTextures(GLuint _w, GLuint _h);

	void SimBody();

	GLuint GetTexture() { return m_textures[current]; }
};