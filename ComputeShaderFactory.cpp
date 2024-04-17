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
	if (_type == "shaders/Life")
		return new Life();
	if (_type == "shaders/PerlinNoise")
		return new PerlinNoise();
	else
	{
		printf("UNKNOWN COMPUTE SHADER TYPE: %s \n", _type.c_str());
		assert(0);
		return nullptr;
	}
}
