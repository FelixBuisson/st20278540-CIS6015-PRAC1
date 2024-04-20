#include "GameObject.h"
#include "cMesh.h"
#include "cTransform.h"

GameObject::GameObject()
{
	m_transform = new cTransform();
	m_type = "GAMEOBJECT";
}

GameObject::~GameObject()
{
	delete m_transform;
}

void GameObject::Load(FILE* _fp)
{
	char buffer[256] = { "\0" };

	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_pos.x, &m_transform->m_pos.y, &m_transform->m_pos.z);					// POS:	0.0 0.0 0.0
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_rot.x, &m_transform->m_rot.y, &m_transform->m_rot.z);					// ROT : 2.0 2.0 0.0
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_rot_incr.x, &m_transform->m_rot_incr.y, &m_transform->m_rot_incr.z);	// ROT_INCR:	1.0 1.0 0.0
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_scale.x, &m_transform->m_scale.y, &m_transform->m_scale.z);			// SCALE : 1.25 1.25 1.25
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_vel.x, &m_transform->m_vel.y, &m_transform->m_vel.z);				// VEL: 0.0  0.0 0.0
	m_transform->print();
}

void GameObject::Tick(GLFWwindow* _window)
{
	m_transform->Update();
}
