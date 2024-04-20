#pragma once

#include "core.h"
#include "cShader.h"
#include <list>

class cMesh;
class cCubeMap;
class IComputeShader;
class Game;
class cLight;

using std::list;

//this is sort of a mixture of a VERY rudamentary scene graph and a basic renderer
//all the GPU facing stuff is here: meshes, shaders, lights and cameras
//each camera effectively has its own render pass
class cScene
{
public:

	cScene()
	{
		m_Meshes = NULL;
		m_CubeMaps = NULL;
	}

	~cScene() {}

	//load in all data from the scene.sdf
	void OpenSDF();

	//once loaded from the scen.sdf initialise all the renderables and other information
	//game is passed in so that we can aquire pointers for Game Objects we might want 
	//to link to
	void Init(float _w, float _h, Game* _game);

	//actually draw all the renderables we have
	void Render();
	void SetGlobalValues(unsigned int _prog);

	//tick all render related items
	void Update(GLFWwindow* _window);

	//toggle wire frame mode 
	void ToggleWireMode() { m_wireMode = !m_wireMode; }

	//so Game Objects can find them
	//return pointers to meshes, lights, Prims and Cameras by their names
	cMesh* FindMesh(string _name);
	cLight* FindLight(string _light);
	cMesh* FindPrim(string _prim);
	cCamera* FindCam(string _cam);

	unsigned int GetShaderProgByName(string _name) { return m_ShaderInfo.GetSPbyName(_name); }

protected:

	//loop through all cameras that can be the main one
	void NextCamera();
	//do the body of the loop for rendering the objects
	void RenderLoop(cCamera* _useCam);

	//data structures for all the rendering elements
	cShaderInfo		m_ShaderInfo;
	list<cCamera*> m_Cameras;
	list<cLight*> m_Lights;
	cMesh* m_Meshes;
	cMesh* m_Prims; // basic primitives used to represent light / camera objects etc
	cCubeMap* m_CubeMaps;
	sShaderDetails* ShaderTable;
	list<IComputeShader*> m_computes;

	list<cCamera*>::iterator m_useCamera; //current main camera in use

	//how many of everything do I have
	int m_shader_count;
	int m_camera_count;
	int m_light_count;
	int m_prim_count;
	int m_mesh_count;
	int m_cubeMap_count;

	//size of the screen
	int m_width, m_height;

	bool m_wireMode = false;
private:
	unsigned int m_tex[32];
	Game* m_game; //keep this around so can access the Game Objects
};