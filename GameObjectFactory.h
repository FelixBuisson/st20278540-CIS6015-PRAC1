#pragma once
#include <string>
class GameObject;
//HACK A rather simple Factory using the base class GameObject
//generates a GameObject based on its type
class GameObjectFactory
{
public:

	static GameObject* makeNewGO(std::string _type);
};

