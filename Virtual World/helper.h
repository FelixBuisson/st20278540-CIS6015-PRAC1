#pragma once
//a useful function that was needed in a few places, so made this mini-helper class
class Helper
{
public:

	static bool SetUniformLocation(unsigned int shader, const char* name, GLint* pLocation)

	{
		*pLocation = glGetUniformLocation(shader, name);

		return (*pLocation >= 0);
	}
};