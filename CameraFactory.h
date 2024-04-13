#pragma once
#include <string>
class cCamera;
//HACK A rather simple Factory using the base class Camera
//generates a Camera based on its type
class CameraFactory
{
public:

	static cCamera* makeNewCam(std::string type);
};