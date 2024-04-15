#include "core.h"
#include <random>
#include <iostream>
#include "life.h"

using namespace std;

void life::GenerateTextures(GLuint _w, GLuint _h) {

	m_loops = 2;
	m_name = "life";
	m_w = _w;
	m_h = _h;
	m_textures = new GLuint[2];

	float* data = new float[_w * _h * 4];
	for (int i = 0; i < _h; i++)
	{
		for (int j = 0; j < _w; j++)
		{
			data[4 * (i * _w + j) + 0] = (((float)rand() / (float)RAND_MAX) > 0.5) ? 1.0f : 0.0f;
			data[4 * (i * _w + j) + 1] = 0.0f;
			data[4 * (i * _w + j) + 2] = 0.0f;
			data[4 * (i * _w + j) + 3] = 1.0f;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		GLuint* newTexture = &m_textures[i];

		glGenTextures(1, newTexture);
		glBindTexture(GL_TEXTURE_2D, *newTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _w, _h, 0, GL_RGBA, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	delete[] data;
}

void life::SimBody()
{
	if (((float)rand() / (float)RAND_MAX) < 0.001)
	{
		glDeleteTextures(2, m_textures);
		delete[] m_textures;
		GenerateTextures(m_w, m_h);
	}

	glUseProgram(m_computeShader);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_data);

	GLuint numGroupsX = m_w >> 4;
	GLuint numGroupsY = m_h >> 4;
	GLuint numGroupsZ = 1;

	glBindImageTexture(0, current ? m_textures[0] : m_textures[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, current ? m_textures[1] : m_textures[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);

	current = (current == 0);
}
