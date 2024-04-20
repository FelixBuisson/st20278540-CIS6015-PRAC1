#include "core.h"
#include <random>
#include <iostream>
#include "CheckerBoard.hpp"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
using namespace glm;
using namespace std;


CheckerBoard::CheckerBoard() {
	m_size = 256;

	m_name = "checkerboard";

}

// The constructor doesn't create the texture - so we do that with a call to generateTexture.  
// Create a new texture object and return the object id
void CheckerBoard::GenerateTextures(GLuint _w, GLuint _h) {

	m_textures = new GLuint;

	glGenTextures(1, m_textures);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);

	// Setup texture data - but pass null for last parameter.  
	// OpenGL creates the texture image array in memory but no data is passed into this.  
	// We'll provide the content when we run our shader.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// Setup texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Bind shader
	glUseProgram(m_computeShader);

	// Bind the texture to image object binding point 0
	glBindImageTexture(0, m_textures[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Calculate number of groups - since the shader declares 16x16 workgroup sizes, we divide the total image / texture size by 16 to get the number of workgroups on the x and y dimensions.  Since it's only a 2D dataset, z has a workgroup size of 1.
	GLuint numGroupsX = _w >> 4;
	GLuint numGroupsY = _h >> 4;
	GLuint numGroupsZ = 1;

	// Run our compute shader to fill in the texture data
	//for a continuously simlating one this is not likely to be needed
	//although you will need to set up your starting state
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}