#include "core.h"
#include "cLight.h"
#include "cMesh.h"

#include "helper.h"

cLight::cLight()
{
	m_type = "LIGHT";
	m_info.m_pos.x = 0.0f;
	m_info.m_pos.y = 0.0f;
	m_info.m_pos.z = 0.0f;
}

void cLight::Load(FILE* _fp)
{
	char buffer[256] = { "\0" };
	char primType[256] = { "\0" };
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_info.m_pos.x, &m_info.m_pos.y, &m_info.m_pos.z); // POS:	-25.0 0.0 20.0
	printf("POS: %f %f %f\n", m_info.m_pos.x, m_info.m_pos.y, m_info.m_pos.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_info.m_col.x, &m_info.m_col.y, &m_info.m_col.z); // COL:	1.0 1.0 1.0
	printf("COL: %f %f %f\n", m_info.m_col.x, m_info.m_col.y, m_info.m_col.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_info.m_att.x, &m_info.m_att.y, &m_info.m_att.z); // ATT:	1.0 0.5 0.2
	printf("ATT: %f %f %f\n", m_info.m_att.x, m_info.m_att.y, m_info.m_att.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_info.m_amb.x, &m_info.m_amb.y, &m_info.m_amb.z); // AMB:	-25.0 0.0 20.0
	printf("AMB: %f %f %f\n", m_info.m_amb.x, m_info.m_amb.y, m_info.m_amb.z);
	fscanf_s(_fp, "%s%s", buffer, 256, primType, 256); // PRIM: which prim to use
	m_primType = primType;
	printf("PRIM: %s\n", primType);
	fscanf_s(_fp, "%s%f", buffer, 256, &m_primScale); //PRIMSCALE: 0.05
	printf("PRIMSCALE: %f\n", m_primScale);
}

/////////////////////////////////////////////////////////////////////////////////////
// Update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cLight::Update(GLFWwindow* _window)
{
}

//prep things to render this light's primative to show where it is
void cLight::PrepPrimRender()
{
	mat4 worldMat = glm::translate(mat4(1.0), vec3(m_info.m_pos));
	worldMat = glm::scale(worldMat, vec3(m_primScale));
	m_pPrimitive->SetWorldMatrix(worldMat);
}

//send values to the shaders to allow the use of this light
// <m_name>Pos <m_name>Col <m_name>Amb
void cLight::SetRenderValues(unsigned int _prog)
{
	GLint loc;
	string posString = m_name + "Pos";
	string colString = m_name + "Col";
	string ambString = m_name + "Amb";

	if (Helper::SetUniformLocation(_prog, posString.c_str(), &loc))
		glUniform3fv(loc, 1, glm::value_ptr(GetPos()));

	if (Helper::SetUniformLocation(_prog, colString.c_str(), &loc))
		glUniform3fv(loc, 1, glm::value_ptr(GetCol()));

	if (Helper::SetUniformLocation(_prog, ambString.c_str(), &loc))
		glUniform3fv(loc, 1, glm::value_ptr(GetAmb()));
}
