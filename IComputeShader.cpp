#include "IComputeShader.h"

//
void IComputeShader::Simulate()
{
	for (int i = 0; i < m_loops; i++)
	{
		SimBody();
	}

	list<GLuint*>::iterator it;
	for (it = m_usedTex.begin(); it != m_usedTex.end(); it++)
	{
		(*(*it)) = GetTexture();
	}
}
