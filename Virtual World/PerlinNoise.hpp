#pragma once
#include "IComputeShader.h"

//
// Model a Perlin noise-based procedural texture
//

class PerlinNoise : public IComputeShader {

	// Buffer to store uniform data that configures the noise
	GLuint			perlinModelBuffer;

public:

	// Constructor to setup the generator
	PerlinNoise();

	// The constructor doesn't create the texture - do we that with a call to generateTexture.  Create a new texture object and return the object id
	virtual void GenerateTextures(GLuint _w, GLuint _h);

	virtual GLuint GetTexture() { return m_textures[0]; }
};