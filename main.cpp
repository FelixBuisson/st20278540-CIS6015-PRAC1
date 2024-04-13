
#include "core.h"
#include <math.h>
#include <time.h> 

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

#include "game.h"
#include "cScene.h"

#include <iostream>

using namespace std;
using namespace glm;

/////////////////////////////////////////////////////////////////////////////////////
// global variables
/////////////////////////////////////////////////////////////////////////////////////

unsigned int initWidth = 800;
unsigned int initHeight = 600;

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void Resize(GLFWwindow* _window, int _width, int _height)
{
	glViewport(0, 0, _width, _height);		// Draw into entire window
}

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void Keyboard(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	// ignore key up (key release) events
	if (_action == GLFW_RELEASE)
		return;

	// process keyboard inputs here..
	switch (_key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(_window, true);
		break;
	default:
	{
	}
	}

}

/////////////////////////////////////////////////////////////////////////////////////
// PrintDebugInfo() - print version info to console
/////////////////////////////////////////////////////////////////////////////////////
void PrintDebugInfo()
{
	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));
	printf("------------------------------\n");
	printf("Press ESCAPE or 'X' or 'x' to exit.\n");
}

/////////////////////////////////////////////////////////////////////////////////////
// DebugOutput
// Used to output openGL debug info
// KHR_debug runs on all systems running openGL4.3 or later
/////////////////////////////////////////////////////////////////////////////////////
void DebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data)
{
	cout << "debug call: " << msg << endl;
}

/////////////////////////////////////////////////////////////////////////////////////
// main()
/////////////////////////////////////////////////////////////////////////////////////
int main()
{

	// initialise glfw
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	srand(time(NULL));

	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, "GraphicsTemplate", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stdout, "Failed to create GLFW window!\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// initialise glew
	glewInit();

	// print version info 
	PrintDebugInfo();

	glDebugMessageCallback(DebugOutput, NULL);

	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, Resize);

	// Set callback for keyboard events
	glfwSetKeyCallback(window, Keyboard);

	Resize(window, initWidth, initHeight);

	////////////////////////////////////////////////////
	// initialise game and scene 
	////////////////////////////////////////////////////
	Game* game = new Game();

	cScene* scene = new cScene();
	scene->Init(initWidth, initHeight, game);

	// Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {

		game->Update(window);			//update game logic
		scene->Update(window);			//update renderables based on update Game
		scene->Render();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		glfwPollEvents();				// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

