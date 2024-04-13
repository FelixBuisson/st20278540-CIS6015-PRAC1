#include "ComputeShaderFactory.h"
#include "ComputeShaders.h"
#include "assert.h"

IComputeShader* ComputeShaderFactory::makeNewCS(std::string _type)
{
	printf("COMPUTE SHADER TYPE: %s \n", _type.c_str());
	if (_type == "shaders/checkerboard")
	{
		return new CheckerBoard();
	}
	else if (_type == "shaders/life")
		return new Life();
	else
	{
		printf("UNKNOWN COMPUTE SHADER TYPE: %s \n", _type.c_str());
		assert(0);
		return nullptr;
	}
}
