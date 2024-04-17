#include "Player.h"
#include "cMesh.h"
#include "cTransform.h"

Player::Player()
{
	m_transform = new cTransform();
	m_type = "PLAYER";
}

Player::~Player()
{
	delete m_transform;
}

void Player::Load(FILE* _fp)
{
	char buffer[256] = { "\0" };

	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_pos.x, &m_transform->m_pos.y, &m_transform->m_pos.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_rot.x, &m_transform->m_rot.y, &m_transform->m_rot.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_rot_incr.x, &m_transform->m_rot_incr.y, &m_transform->m_rot_incr.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_scale.x, &m_transform->m_scale.y, &m_transform->m_scale.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_transform->m_vel.x, &m_transform->m_vel.y, &m_transform->m_vel.z);;
}

void Player::Tick(GLFWwindow* _window)
{
	vec3 updatedPosition = vec3(0.0f, 0.0f, 0.0f);
	vec3 updatedRotation = vec3(0.0f, 0.0f, 0.0f);

	double mousePosX;
	double mousePosY;
	glfwGetCursorPos(_window, &mousePosX, &mousePosY);

	double movedMousePosX = (mousePosX - m_previousMouseXPos);
	double movedMousePosY = (mousePosY - m_previousMouseYPos);

	m_previousMouseXPos = mousePosX;
	m_previousMouseYPos = mousePosY;

	updatedRotation = vec3(movedMousePosY, -movedMousePosX, 0.0f);

	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_W) == GLFW_REPEAT)
		updatedPosition.z += 0.01f;
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_A) == GLFW_REPEAT)
		updatedPosition.x += 0.01f;
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_S) == GLFW_REPEAT)
		updatedPosition.z -= 0.01f;
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_D) == GLFW_REPEAT)
		updatedPosition.x -= 0.01f;
	if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_REPEAT)
		updatedPosition.y -= 0.01f;
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_REPEAT)
		updatedPosition.y += 0.01f;

	m_transform->Update(updatedPosition, updatedRotation);
}