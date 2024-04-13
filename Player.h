#pragma once
#include <stdio.h>
#include <string>
#include "GameObject.h"

using namespace std;
class cMesh;
class cTransform;
struct GLFWwindow;

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();
	virtual void Load(FILE* fp);
	virtual void Tick(GLFWwindow* _window);
	double getPrevMousePosX() { return m_previousMouseXPos; }
	double getPrevMousePosY() { return m_previousMouseYPos; }

protected:
	double m_previousMouseXPos = 0.0f;
	double m_previousMouseYPos = 0.0f;

};