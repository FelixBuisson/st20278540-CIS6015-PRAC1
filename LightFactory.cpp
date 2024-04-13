#include "LightFactory.h"
#include <assert.h>
#include "cLight.h"

cLight* LightFactory::makeNewLight(std::string _type)
{
	printf("LIGHT TYPE: %s \n", _type.c_str());
	if (_type == "LIGHT")
	{
		return new cLight();
	}
	else
	{
		printf("UNKNOWN LIGHT TYPE!");
		assert(0);
		return nullptr;
	}
}
