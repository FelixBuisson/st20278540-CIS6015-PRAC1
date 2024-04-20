#pragma once
#include <list>
#include <string>
#include <stdio.h>

using namespace std;

class GameObject;
class cScene;
struct GLFWwindow;

class Game
{
public:
	Game();
	//tick all GOs
	//_window allows for keyboard access
	void Update(GLFWwindow* _window);

	//add this GO to my list
	void AddGameObject(GameObject* _new);

	//return a pointer to a given GO by its name
	GameObject* GetGameObject(string _GOName);

	//load the GameObject Descriptions from the SDF file
	void OpenSDF(FILE* _fp, cScene* _scene);

	//Don't like this being here but it allows multi-use of meshes
	void RenderMeshes(class cCamera* _cam, cScene* _scene);

protected:

	//data structure containing pointers to all my GameObjects
	std::list<GameObject*> m_GameObjects;

};

