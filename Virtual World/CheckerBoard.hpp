#pragma once
#include "IComputeShader.h"

//
// Procedurally generate a CheckerBoard Pattern
//

class CheckerBoard : public IComputeShader {

public:

	// Constructor to setup the Checkerboard
	CheckerBoard();

	//Generate the textures required for this  
	virtual void GenerateTextures(GLuint _w, GLuint _h);

	//return the current texture this compute shader is outputing
	virtual GLuint GetTexture() { return m_textures[0]; }
};