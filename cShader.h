//////////////////////////////////////////////////////////////////////////////////////////
// cShader.h
//////////////////////////////////////////////////////////////////////////////////////////
//loads from various source files shaders, then compiles them
//and supplies the handles to make use of them in the main renderer
//this is largely from the original codebase is seems to be a mish-mash of about three previous versions
//spliting the renderer and the game aspects off added the factories were main tidy up func task I did next year
//next year I'll tidy this bit up too!
#pragma once
#include <string>
#include <list>
using std::string;
using std::list;

class IComputeShader;

class cShader
{
public:

	cShader();
	~cShader() {}

	inline void	 Cwd(char* _path) { strcpy_s(m_cwd, sizeof(char) * 32, _path); }
	inline char* Cwd() { return m_cwd; }

	inline unsigned int	program() { return m_program; }

	void CheckShader(GLuint _id, GLuint _type, GLint* _ret, const char* _onfail);
	void Clean();
	void Create(struct sShaderDetails* _pShader, list<IComputeShader*>* _computes);
	void LoadShaderFile(struct sShaderDetails* _pShader, int _count);

	bool HasTess() { return m_bhasTess; } //does this shader have a tesselation component
	bool hasName(string _name) { return ("shaders/" + _name == m_name); }

private:
	bool m_bhasTess = false;

	char* m_cwd;

	GLuint m_vs;	// vertex shader
	GLuint m_cs;	// tesselation control shader
	GLuint m_es;	// tesselation evaluation shader
	GLuint m_fs;	// fragment shader
	GLuint m_gs;	// geometry shader
	GLuint m_comp;	// compute shader

	unsigned int m_program;

	string m_name;

	char* TextFileRead(char* _fn);
};

//HACK okay we always use sShaderDetails and cShaderInfo with cShader
//but really don't like them sharing files like this
struct sShaderDetails
{
	char filename[256];
	unsigned long types[6];
};

class cShaderInfo
{
public:

	cShaderInfo();
	~cShaderInfo() {}
	void Create(int _count, sShaderDetails* _ShaderTable, list<IComputeShader*>* _computes);
	void Clean();
	bool SetUniformLocation(unsigned int _shader, const char* _name, GLint* _pLocation);

	inline cShader* GetList() { return m_pList; }

	unsigned int GetSPbyName(string _name);
	bool HasTessByName(string _name);

	class cShader* List(int _id);

	void Set_flags(unsigned int _value)
	{
		m_flags = _value;
	}

	unsigned int Get_flags()
	{
		return m_flags;
	}


private:
	class cShader* m_pList;

	int				m_num_shaders;
	unsigned int	m_flags;		// used to render individual maps

};
