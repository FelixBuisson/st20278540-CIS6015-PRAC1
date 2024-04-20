#pragma once
#include <string>
class cLight;

//ditto for the other factory but now for lights!
class LightFactory
{
public:

	static cLight* makeNewLight(std::string _type);
};

