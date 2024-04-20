#pragma once
#include "core.h"

//you can have up to four cube maps which are provided globally
class cCubeMap
{
public:
	cCubeMap();
	~cCubeMap();
	void Init();

	char** m_tex;

	unsigned int m_Cubemap;
};
