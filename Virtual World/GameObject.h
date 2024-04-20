#pragma once
#include <stdio.h>
#include <string>

using namespace std;
class cMesh;
class cTransform;
struct GLFWwindow;

//Base class of a GameObject
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	//load me from the SDF
	virtual void Load(FILE* fp);

	//update _window allows for Keyboard access
	virtual void Tick(GLFWwindow* _window);

	//various getters and setters
	void SetMesh(cMesh* _mesh) { m_mesh = _mesh; }
	cMesh* GetMesh() { return m_mesh; }
	void SetName(string _name) { m_name = _name; }
	string GetName() { return m_name; }
	cTransform* GetTransform() { return m_transform; }

protected:

	string m_name;
	string m_type;

	cMesh* m_mesh = nullptr;
	cTransform* m_transform = nullptr;
};

