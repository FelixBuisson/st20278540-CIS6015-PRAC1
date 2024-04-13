#pragma once
#include <string>
class IComputeShader;

//HACK A rather simple Factory using the base class IComputeShaders
//generates the driver objects for each type of compute shader based on its filename
//i.e. shaders/checkerboard
class ComputeShaderFactory
{
public:

	static IComputeShader* makeNewCS(std::string _type);
};

