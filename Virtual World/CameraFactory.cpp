#include "CameraFactory.h"
#include "cCamera.h"
#include <assert.h>
#include "cCameraLookat.h"

using std::string;

cCamera* CameraFactory::makeNewCam(string _type)
{
	printf("CAM TYPE: %s \n", _type.c_str());
	if (_type == "CAMERA")
	{
		return new cCamera();
	}
	else if (_type == "CAMERALOOKAT")
		return new cCameraLookAt;
	else
	{
		printf("UNKNOWN CAMERA TYPE!");
		assert(0);
		return nullptr;
	}
}
