#pragma once

#include "core.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

using namespace glm;
//a Transform class used by GameObjects to tell meshes where to render themselves
class cTransform
{
public:
	cTransform()
	{
		m_worldMatrix = mat4(1.0f);

		m_rot = vec3(0, 0, 0);
		m_pos = vec3(0, 0, 0);
		m_scale = vec3(1, 1, 1);
		m_rot_incr = vec3(0, 0, 0);
	}
	~cTransform()
	{
	}

	//update using the m_rot_incr and m_vel
	//base movement and spinning
	void Update();
	void Update(vec3 _position, vec3 _rotation);

	vec3		m_pos;
	vec3		m_rot;		// current rotation angle used to Create the rotation matrix
	vec3		m_rot_incr;	// rotation incrementor
	vec3		m_scale;
	vec3		m_vel;

	vec3		m_PreviousRot;
	float		m_forwardDirection = 0.0f;
	glm::mat3	m_NormalMatrix;
	glm::mat4	m_ModelView;
	glm::mat4	m_ModelViewProj;

	//print out the values of this transform
	void print();

	glm::mat4	m_worldMatrix;			// world matrix for placing the mesh into the world

	void SetPosition(vec3 _position) { m_pos = _position; }
	vec3 GetPosition() { return m_pos; }
	void SetRotation(vec3 _rotation) { m_rot = _rotation; }
	vec3 GetRotation() { return m_rot; }
	vec3 GetPreviousRotation() { return m_PreviousRot; }
};





