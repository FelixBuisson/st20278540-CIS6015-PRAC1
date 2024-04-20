#pragma once
#define GLEW_STATIC
#include "GL/glew.h" 
#include "GLFW/glfw3.h"
#include <string>
#include <list>

using namespace std;

//Base class for Compute Shaders
class IComputeShader
{
protected:
	// Shader program
	GLuint m_computeShader = -1;

	//textures
	GLuint* m_textures = NULL;

	int m_w = 0;
	int m_h = 0;

	//how many times do I loop my simulation
	int m_loops = 0;

	//name of this compute shader
	string m_name = "";

	//used when we have more than one texture
	list<GLuint*> m_usedTex;

	//default size for the textures for this compute shader
	int m_size = 0;

public:

	// Constructor to setup the generator
	IComputeShader() {};

	// The constructor doesn't create the texture - do that with a call to generateTexture.  
	// Create a new texture object and return the object id
	virtual void GenerateTextures(GLuint _w, GLuint _h) = 0;

	//when we compile a compute shader we'll make an object inheriting from
	//IComputerShader to driev it this is how we tell it what Shader to use 
	void SetShaderProgram(GLuint _inSP) { m_computeShader = _inSP; }

	//run the simulation of this compute shader
	//essentially the tick
	virtual void Simulate();

	//possibly need to loop the base sim multiple times in one tick
	//body of sim goes here
	virtual void SimBody() {};

	//give acces to the current output texture for this compute shader
	virtual GLuint GetTexture() = 0;

	//hello my name is
	string GetName() { return m_name; }

	//provide me with a texture
	void AddUsedTex(unsigned int* _tex) { m_usedTex.push_back(_tex); }

	//you are called
	void SetName(string _name) { m_name = _name; }

	//how big is my texture
	int GetSize() { return m_size; }
};

