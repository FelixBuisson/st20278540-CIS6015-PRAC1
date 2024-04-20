#pragma once
#include "cCamera.h"
#include "GameObject.h"

using std::string;

using namespace glm;

class cTransform;
class cLight;
class Game;
class cScene;
class cMesh;

//base class for a camera

class cCameraLookAt : public cCamera
{
public:
	cCameraLookAt();
	~cCameraLookAt();

	//initialise the camera _width _height
	// game and scene maybe needed for more involved cameras to connect to relvant GOs and lights/shaders
	virtual void Init(float _w, float _h, Game* _game, cScene* _scene);

	virtual void Update(cCamera* _main);

	void GetAngleAroundGameObject();

	//load camera info from the SDF
	virtual void Load(FILE* _fp, bool loadPrimName = true);

	GameObject* m_lookatGameObject = nullptr;
	string m_lookatGameObjectName = "";
	float angleAroundPlayer;

};