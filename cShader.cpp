#include "core.h"
#include "cShader.h"
#include "assert.h"
#include <windows.h>
#include <direct.h>
#include <iostream>
#include <string>
#include "ComputeShaderFactory.h"
#include "IComputeShader.h"

///////////////////////////////////////////////////////////////////////
// Number of programmable shaders available. 
// Note:	The order of the ShaderTypes reflects the order within the
//			ShaderList[] - see below
///////////////////////////////////////////////////////////////////////
const int MAX_SHADERS_TYPES = 6;

GLuint ShaderTypes[MAX_SHADERS_TYPES] =
{
	GL_VERTEX_SHADER,				// vs
	GL_TESS_CONTROL_SHADER,			// cs
	GL_TESS_EVALUATION_SHADER,		// es
	GL_GEOMETRY_SHADER,				// gs
	GL_FRAGMENT_SHADER,				// fs

	// Note: GL_COMPUTE_SHADER is a standalone shader
	GL_COMPUTE_SHADER,				// comp
};

//////////////////////////////////////////////////////////////////////////////////////////
// cShader() - constructor
//////////////////////////////////////////////////////////////////////////////////////////
cShader::cShader()
{
	m_cwd = new char[256];

	m_program = -1;
	m_vs = -1;
	m_fs = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Clean() - delete shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::Clean()
{
	if (m_vs)
		glDeleteShader(m_vs);
	if (m_fs)
		glDeleteShader(m_fs);
	if (m_cs)
		glDeleteShader(m_cs);
	if (m_es)
		glDeleteShader(m_es);
	if (m_gs)
		glDeleteShader(m_gs);
	if (m_comp)
		glDeleteShader(m_comp);
	if (m_program)
		glDeleteProgram(m_program);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CheckShader() - creates if anything went wrong in shader compile/linking
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::CheckShader(GLuint _id, GLuint _type, GLint* _ret, const char* _onfail)
{
	switch (_type)
	{
	case(GL_COMPILE_STATUS):
	{
		glGetShaderiv(_id, _type, _ret);
		if (*_ret == false)
		{
			int infologLength = 0;
			glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infologLength);
			GLchar buffer[512];
			GLsizei charsWritten = 0;
			std::cout << _onfail << std::endl;
			glGetShaderInfoLog(_id, infologLength, &charsWritten, buffer);
			std::cout << buffer << std::endl;
			assert(0);
		}
	}
	break;
	case(GL_LINK_STATUS):
	{
		glGetProgramiv(_id, _type, _ret);
		if (*_ret == false) {
			int infologLength = 0;
			glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infologLength);
			GLchar buffer[512];
			GLsizei charsWritten = 0;
			std::cout << _onfail << std::endl;
			glGetProgramInfoLog(_id, infologLength, &charsWritten, buffer);
			std::cout << buffer << std::endl;
			assert(0);
		}
	}
	break;
	default:
		break;
	};
}

/////////////////////////////////////////////////////////////////////////////////////
// LoadShaderFile() - recursively loops through and loads all shader files associated
//					  with this shader
/////////////////////////////////////////////////////////////////////////////////////
void cShader::LoadShaderFile(sShaderDetails* _pShader, int _count)
{
	if (_count >= MAX_SHADERS_TYPES)
		return;

	if (_pShader->types[_count])
	{
		char file_type[8] = { "\0" };

		GLuint pShaderID;

		bool addToComputes = false;

		switch (_pShader->types[_count])
		{
		case GL_VERTEX_SHADER: 			strcpy_s(file_type, sizeof(char) * 8, ".vs");	break;
		case GL_FRAGMENT_SHADER:		strcpy_s(file_type, sizeof(char) * 8, ".fs");	break;
		case GL_TESS_CONTROL_SHADER:	strcpy_s(file_type, sizeof(char) * 8, ".cs");	break;
		case GL_TESS_EVALUATION_SHADER:	strcpy_s(file_type, sizeof(char) * 8, ".es");	break;
		case GL_GEOMETRY_SHADER:		strcpy_s(file_type, sizeof(char) * 8, ".gs");	break;
		case GL_COMPUTE_SHADER:			strcpy_s(file_type, sizeof(char) * 8, ".comp");	addToComputes = true;  break;
		}

		pShaderID = glCreateShader(_pShader->types[_count]);

		char fp[256];
		char file[256];
		strcpy_s(file, sizeof(char) * 256, _pShader->filename);
		strcat_s(file, sizeof(char) * 256, file_type);

		strcpy_s(fp, sizeof(char) * 256, m_cwd);
		strcat_s(fp, sizeof(char) * 256, "\\");
		strcat_s(fp, sizeof(char) * 256, file);


		char* s = TextFileRead(file);

		if (!s)
		{
			printf("\n\nERROR: file: %s not found -  ", file);
			return;
		}

		GLint ret;

		glShaderSource(pShaderID, 1, &s, NULL);

		free(s);
		printf("Compiling %s%s\n", _pShader->filename, file_type);
		glCompileShader(pShaderID);
		CheckShader(pShaderID, GL_COMPILE_STATUS, &ret, "unable to compile the shader!");

		glAttachShader(m_program, pShaderID);
	}

	LoadShaderFile(_pShader, ++_count);
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////
// TextFileRead() - 
//////////////////////////////////////////////////////////////////////////////////////////
// takend fromtextfile.cpp
//
// simple reading and writing for text files
//
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//////////////////////////////////////////////////////////////////////
char* cShader::TextFileRead(char* _fn)
{
	FILE* fp;
	char* content = NULL;

	size_t count = 0;

	if (_fn != NULL)
	{
		fopen_s(&fp, _fn, "rt");

		if (fp != NULL)
		{

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0)
			{
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}

	return content;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Create() - creates a new shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::Create(sShaderDetails* _pShader, list<IComputeShader*>* _computes)
{
	m_name = _pShader->filename;

	char* cwd = _getcwd(m_cwd, _MAX_PATH);

	m_program = glCreateProgram();

	LoadShaderFile(_pShader, 0);

	if (_pShader->types[1] == GL_TESS_CONTROL_SHADER || _pShader->types[2] == GL_TESS_EVALUATION_SHADER) m_bhasTess = true;

	GLint ret;

	glLinkProgram(m_program);
	CheckShader(m_program, GL_LINK_STATUS, &ret, "linkage failure!");

	if (_pShader->types[5] == GL_COMPUTE_SHADER)
	{
		IComputeShader* pCS = ComputeShaderFactory::makeNewCS(_pShader->filename);
		pCS->SetShaderProgram(m_program);
		int size = pCS->GetSize();
		pCS->GenerateTextures(size, size);
		_computes->push_back(pCS);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// cShaderInfo() - constructor
//////////////////////////////////////////////////////////////////////////////////////////
cShaderInfo::cShaderInfo()
{
	m_pList = NULL;
	m_flags = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
// Create() - allocate block of memory to hold ALL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::Create(int _count, sShaderDetails* _ShaderTable, list<IComputeShader*>* _computes)
{
	m_num_shaders = _count;
	m_pList = new class cShader[_count];

	for (int i = 0; i < _count; i++)
	{
		List(i)->Create(&_ShaderTable[i], _computes);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
// SetUniformLocation() - return the uniform location if one exists
//////////////////////////////////////////////////////////////////////////////////////////
bool cShaderInfo::SetUniformLocation(unsigned int shader, const char* name, GLint* pLocation)
{
	*pLocation = glGetUniformLocation(shader, name);

	return (*pLocation >= 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Clean() - delete an existing shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::Clean()
{
	if (m_pList)
	{
		for (int i = 0; i < m_num_shaders; i++)
		{
			List(i)->Clean();
		}
	}
}

unsigned int cShaderInfo::GetSPbyName(string _name)
{
	for (int i = 0; i < m_num_shaders; i++)
	{
		if (List(i)->hasName(_name))
		{
			return List(i)->program();
		}
	}
	printf("UNKNOWN SHADER : %s\n", _name.c_str());
	assert(0);

	return 0;
}

bool cShaderInfo::HasTessByName(string _name)
{
	int found = 0;
	for (int i = 0; i < m_num_shaders; i++)
	{
		if (List(i)->hasName(_name))
		{
			return List(i)->HasTess();
		}
	}
	printf("UNKNOWN SHADER : %s\n", _name.c_str());
	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// List() - returns requested element from shader List
//////////////////////////////////////////////////////////////////////////////////////////
cShader* cShaderInfo::List(int id)
{
	if (id >= m_num_shaders)
	{
		// if you get here, then you are trying to access a shader element that exceeds the shader array size.
		// try increasing the number_of_shaders created in main() - ShaderInfo.Create(number_of_shaders);
		printf("ERROR: ");
		printf("array limits exceeded\n");
	}

	return &m_pList[id];
}

