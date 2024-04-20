#pragma once
#include <string>

using std::string;

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

class cMesh;
struct GLFWwindow;

struct sLightInfo
{
	vec3 m_pos; // position of the light
	vec3 m_col; // colour of the light
	vec3 m_amb; // ambient colour of the light
	vec3 m_att; // attenuation coefficients
};

//base class for a light
class cLight
{
public:
	cLight();
	~cLight() {}

	void Init(float _x, float _y, float _z)
	{
		m_info.m_pos.x = _x;
		m_info.m_pos.y = _y;
		m_info.m_pos.z = _z;
	}

	//load from SDF
	virtual void Load(FILE* fp);

	//tick this light
	//as for Game Objects bring in _window to allow for keyboard access
	virtual void Update(GLFWwindow* _window);

	//Getters and Setters
	void SetName(string _name) { m_name = _name; }
	string GetName() { return m_name; }
	void SetTye(string _type) { m_type = _type; }
	string GetType() { return m_type; }

	vec3 GetCol() { return m_info.m_col; }
	vec3 GetAmb() { return m_info.m_amb; }

	cMesh* GetPrim() { return m_pPrimitive; }
	string GetPrimType() { return m_primType; }
	void SetPrim(cMesh* _prim) { m_pPrimitive = _prim; }
	vec3 GetPos() { return m_info.m_pos; }
	void PrepPrimRender();

	//set my shader values
	//base version if name of light is LG
	//sets up shader values for LGpos LGcol & LGamb
	//position, main colour and ambient colour for this light
	virtual void SetRenderValues(unsigned int _prog);

protected:
	string m_name;
	string m_type;

	sLightInfo m_info;

	//info about my prim to show where I am in the scene
	string m_primType;
	cMesh* m_pPrimitive = nullptr;
	float m_primScale = 1.0f;

};