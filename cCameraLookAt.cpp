#include "cCameraLookat.h"
#include "Game.h"
#include "GameObject.h"
#include "cTransform.h"

/////////////////////////////////////////////////////////////////////////////////////
// constructor
/////////////////////////////////////////////////////////////////////////////////////
cCameraLookAt::cCameraLookAt()
{
	m_type = "CAMERA";
	m_PB = true;
}

/////////////////////////////////////////////////////////////////////////////////////
// destructor
/////////////////////////////////////////////////////////////////////////////////////
cCameraLookAt::~cCameraLookAt()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Init() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCameraLookAt::Init(float _screenWidth, float _screenHeight, Game* _game, cScene* _scene)
{
	float aspect_ratio = _screenWidth / _screenHeight;
	m_projectionMatrix = glm::perspective(glm::radians(m_fov), aspect_ratio, m_near, m_far);
	m_lookatGameObject = _game->GetGameObject(m_lookatGameObjectName);
}

/////////////////////////////////////////////////////////////////////////////////////
// Update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCameraLookAt::Update(cCamera* _main)
{
	vec3 lookatGameObjectPos = m_lookatGameObject->GetTransform()->GetPosition();
	SetLookAt(lookatGameObjectPos);

	m_pos = lookatGameObjectPos;

	vec3 offset = vec3(0.0f, 3.0f, -5.0f);

	//Not currently clamped, come back to this
	float clampedX = m_lookatGameObject->GetTransform()->GetRotation().x;

	mat4 rot = glm::rotate(mat4(1.0), glm::radians(m_lookatGameObject->GetTransform()->GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(clampedX), glm::vec3(1.0f, 0.0f, 0.0f));

	m_pos += (mat3)rot * offset;

	m_viewMatrix = glm::lookAt(m_pos, m_lookAt, vec3(0, 1, 0));

}

void cCameraLookAt::Load(FILE* _fp, bool loadPrimName)
{
	char name[256] = { "\0" };
	char buffer[256] = { "\0" };
	fscanf_s(_fp, "%s%s", buffer, 256, name, 256);		// GAMEOBJECT
	m_lookatGameObjectName = (string)name;
	fscanf_s(_fp, "%s%f", buffer, 256, &m_fov);		// FOV:	45.0
	fscanf_s(_fp, "%s%f", buffer, 256, &m_near);	// NEAR:	0.5
	fscanf_s(_fp, "%s%f", buffer, 256, &m_far);		// FAR:	100.0
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_pos.x, &m_pos.y, &m_pos.z);		// POS:		0.0 0.0 -5.0
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_lookAt.x, &m_lookAt.y, &m_lookAt.z);		// LOOKAT:	0.0 0.0 0.0
	if (loadPrimName)
	{
		char primType[256] = { "\0" };
		fscanf_s(_fp, "%s%s", buffer, 256, primType, 256); // PRIM: which prim to use
		m_primType = primType;
		fscanf_s(_fp, "%s%f", buffer, 256, &m_primScale); //PRIMSCALE: 0.05
	}
}
