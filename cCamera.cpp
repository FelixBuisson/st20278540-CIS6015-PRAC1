#include "cCamera.h"
#include "cTransform.h"
#include "helper.h"
#include "cMesh.h"

/////////////////////////////////////////////////////////////////////////////////////
// constructor
/////////////////////////////////////////////////////////////////////////////////////
cCamera::cCamera()
{
	m_type = "CAMERA";
	m_PB = true;
}

/////////////////////////////////////////////////////////////////////////////////////
// destructor
/////////////////////////////////////////////////////////////////////////////////////
cCamera::~cCamera()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Init() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCamera::Init(float _screenWidth, float _screenHeight, Game* _game, cScene* _scene)
{
	float aspect_ratio = _screenWidth / _screenHeight;
	m_projectionMatrix = glm::perspective(glm::radians(m_fov), aspect_ratio, m_near, m_far);
}

/*
mat4 LookAt(vec3 eye, vec3 at, vec3 up)
{
	vec3 zaxis = normalize(at - eye);
	vec3 xaxis = normalize(cross(zaxis, up));
	vec3 yaxis = cross(xaxis, zaxis);

	zaxis = -zaxis;

	mat4 viewMatrix = {
		vec4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)),
		vec4(yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)),
		vec4(zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)),
		vec4(0, 0, 0, 1)
	};

	return viewMatrix;
}
*/

/////////////////////////////////////////////////////////////////////////////////////
// Update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCamera::Update(cCamera* _main)
{
	m_viewMatrix = glm::lookAt(m_pos, m_lookAt, vec3(0, 1, 0));
}

void cCamera::Load(FILE* _fp, bool loadPrimName)
{
	char buffer[256] = { "\0" };
	fscanf_s(_fp, "%s%f", buffer, 256, &m_fov);		// FOV:	45.0
	printf("FOV: %f\n", m_fov);
	fscanf_s(_fp, "%s%f", buffer, 256, &m_near);	// NEAR:	0.5
	printf("NEAR: %f\n", m_near);
	fscanf_s(_fp, "%s%f", buffer, 256, &m_far);		// FAR:	100.0
	printf("FAR: %f\n", m_far);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_pos.x, &m_pos.y, &m_pos.z);		// POS:		0.0 0.0 -5.0
	printf("POS: %f %f %f\n", m_pos.x, m_pos.y, m_pos.z);
	fscanf_s(_fp, "%s%f%f%f", buffer, 256, &m_lookAt.x, &m_lookAt.y, &m_lookAt.z);		// LOOKAT:	0.0 0.0 0.0
	printf("LOOKAT: %f %f %f\n", m_lookAt.x, m_lookAt.y, m_lookAt.z);
	if (loadPrimName)
	{
		char primType[256] = { "\0" };
		fscanf_s(_fp, "%s%s", buffer, 256, primType, 256); // PRIM: which prim to use
		printf("PRIM: %s\n", primType);
		m_primType = primType;
		fscanf_s(_fp, "%s%f", buffer, 256, &m_primScale); //PRIMSCALE: 0.05
		printf("PRIMSCALE: %f\n", m_primScale);
	}
}

bool cCamera::RenderSetup()
{
	//set things up ready to ready to render with a basic camera
	glBindFramebuffer(GL_FRAMEBUFFER, GetFBN()); //will be zero for main as doesn't do this stuff

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);

	// Clear the rendering window
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	return false;
}

//set things up to render my primitive to shwo where I am in the scene
void cCamera::PrepPrimRender()
{
	mat4 worldMat = glm::inverse(m_viewMatrix);
	worldMat = glm::scale(worldMat, vec3(m_primScale));
	m_pPrimitive->SetWorldMatrix(worldMat);
}

//set the base render values for this camera in the shaders
void cCamera::SetRenderValues(unsigned int _prog)
{
	GLint loc;

	//matrix for the view transform
	if (Helper::SetUniformLocation(_prog, "viewMatrix", &loc))
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(GetView()));

	//matrix for the projection transform
	if (Helper::SetUniformLocation(_prog, "projMatrix", &loc))
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(GetProj()));

	//the current camera is at this position
	if (Helper::SetUniformLocation(_prog, "camPos", &loc))
		glUniform3fv(loc, 1, glm::value_ptr(GetPos()));
}

//if I am a TV camera or similar I'll need to have my own Render Target
//this function will create one
void cCamera::MakeRenderTarget(float _screenWidth, float _screenHeight)
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	//GLuint FramebufferName = 0;
	glGenFramebuffers(1, &m_framebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);

	// The texture we're going to render to
	//GLuint renderedTexture;
	glGenTextures(1, &m_renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, m_renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _screenWidth, _screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	//GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &m_depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _screenWidth, _screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error: Problem with the render target stuff.\n");
	}
}


