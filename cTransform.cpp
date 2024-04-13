#include "core.h"
#include "cTransform.h"

using namespace glm;


//////////////////////////////////////////////////////////////////////////////////////////
// Update() - 
//////////////////////////////////////////////////////////////////////////////////////////
void cTransform::Update()
{
	m_rot += m_rot_incr;

	m_worldMatrix = glm::translate(mat4(1.0), vec3(m_pos));
	m_worldMatrix = glm::rotate(m_worldMatrix, glm::radians(m_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_worldMatrix = glm::rotate(m_worldMatrix, glm::radians(m_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_worldMatrix = glm::rotate(m_worldMatrix, glm::radians(m_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	m_worldMatrix = glm::scale(m_worldMatrix, glm::vec3(m_scale));
}

void cTransform::Update(vec3 _position, vec3 _rotation)
{
	m_rot += _rotation;
	m_PreviousRot = m_rot;
	//m_pos += _position;

	mat4 rot = glm::rotate(mat4(1.0), glm::radians(m_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_pos += (mat3)rot * _position;

	Update();
}

void cTransform::print()
{
	printf("POS: %f %f %f\n", m_pos.x, m_pos.y, m_pos.z);
	printf("ROT: %f %f %f\n", m_rot.x, m_rot.y, m_rot.z);
	printf("ROT_INCR: %f %f %f\n", m_rot_incr.x, m_rot_incr.y, m_rot_incr.z);
	printf("SCALE: %f %f %f\n", m_scale.x, m_scale.y, m_scale.z);
	printf("VEL: %f %f %f\n", m_vel.x, m_vel.y, m_vel.z);
}

