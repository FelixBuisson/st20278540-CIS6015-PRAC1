#pragma once
#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"
#include "core.h"
#include <vector>
#include <string>

using std::string;

using namespace glm;

class cTransform;
class cLight;
class Game;
class cScene;
class cMesh;

//base class for a camera

class cCamera
{
public:
	cCamera();
	~cCamera();

	//initialise the camera _width _height
	// game and scene maybe needed for more involved cameras to connect to relvant GOs and lights/shaders
	virtual void Init(float _w, float _h, Game* _game, cScene* _scene);

	//tick this camera
	virtual void Update(cCamera* _main);

	//load camera info from the SDF
	//only load the data for the primitive if loadPrimName = true
	virtual void Load(FILE* _fp, bool loadPrimName = true);

	//getters
	string GetType() { return m_type; }

	glm::mat4 GetProj() { return m_projectionMatrix; }
	glm::mat4 GetView() { return m_viewMatrix; }

	glm::vec3 GetPos() { return m_pos; }

	float GetFOV() { return m_fov; }
	float GetNear() { return m_near; }
	float GetFar() { return m_far; }

	string GetName() { return m_name; }
	void SetName(string _name) { m_name = _name; }

	void Move(glm::vec3 _d) { m_pos += _d; }

	//if this camera is drawing to a render target these will be needed by the main render loop
	GLuint GetFBN() { return m_framebufferName; }
	GLuint GetRT() { return m_renderedTexture; }

	//where am I looking at
	vec3 GetLookAt() { return m_lookAt; }
	void SetLookAt(vec3 _pos) { m_lookAt = _pos; }

	//our renderpasses are based on cameras for this simple render
	//this sets up how to render for this type of camera
	virtual bool RenderSetup(); //returns depthPass

	//should we push this type of camera to the back of the list or put it on the front
	//cameras that can be main are likely to go to the back
	bool PushBack() { return m_PB; }

	//these are to do with the primitive object we use to show where this camera is located
	cMesh* GetPrim() { return m_pPrimitive; }
	string GetPrimType() { return m_primType; }
	void SetPrim(cMesh* _prim) { m_pPrimitive = _prim; }
	void PrepPrimRender();

	//is this a camera that can be the main view?
	bool CanBeMain() { return m_canBeMain; }

	//if we are using this for a custom render pass
	//it might need its own shader instead of the main ones
	bool HasCustomShader() { return m_hasCustomShader; }
	unsigned int GetCustomShader() { return m_shaderProg; }
	bool HasCustomShaderTess() { return m_shaderHasTess; }

	//set up shader values for when using this camera in its own render pass
	virtual void SetRenderValues(unsigned int _prog);

	//set up shader values in everyone elses render pass
	virtual void SetGlobalRenderValues(unsigned int _prog) {};

protected:
	//construct a render target for this camera
	void MakeRenderTarget(float _screenWidth, float _screenHeight);

	//standard transforms needed to render with this a basic camera
	glm::mat4 m_projectionMatrix;		// projection matrix
	glm::mat4 m_viewMatrix;			// view matrix

	//my camera is here
	glm::vec3 m_pos;

	//Location this camera is looking at
	glm::vec3 m_lookAt;

	float m_fov; //field of view
	float m_near;//near plane distance
	float m_far;//far plane distance

	string m_name;
	string m_type;

	bool m_PB; //see bool PushBack()

	//Frame Buffer stuff for multiple render targets
	GLuint m_framebufferName = 0;
	GLuint m_renderedTexture;
	GLuint m_depthrenderbuffer;

	//details of the primitive used to show this camera
	string m_primType = "";
	cMesh* m_pPrimitive = nullptr;
	float m_primScale = 1.0f;
	bool m_canBeMain = true;

	//if I have a custom shader
	//these will be needed
	bool m_hasCustomShader = false;
	unsigned int m_shaderProg = 0;
	bool m_shaderHasTess = false;
};

