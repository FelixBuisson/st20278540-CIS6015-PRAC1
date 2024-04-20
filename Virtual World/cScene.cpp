#include "core.h"
#include "Game.h"
#include "cScene.h"

#include "cCubeMap.h"
#include "cMesh.h"
#include "ComputeShaders.h"

#include "cLight.h"
#include "LightFactory.h"
#include "cCamera.h"
#include "CameraFactory.h"

/////////////////////////////////////////////////////////////////////////////////////
// Update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::Update(GLFWwindow* _window)
{
	//this keeps track of the previous frames state show this keybaord press only works the first frame the key is pressed
	//stops annoying flicker 
	//this (but without the prevState stuff) is probably the best way to do it for player movement

	//step through suitable cameras by pressing TAB
	static int prevTabState = GLFW_RELEASE;
	if (glfwGetKey(_window, GLFW_KEY_TAB) == GLFW_PRESS && prevTabState != GLFW_PRESS)
	{
		NextCamera();
	}
	prevTabState = glfwGetKey(_window, GLFW_KEY_TAB);

	//update all the cameras
	//pass down the current main one as you are likely to need that for reflection cameras
	for (list<cCamera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		(*it)->Update((*m_useCamera));
	}

	//update all lights
	//pass down the _window to allow for movement of special lights
	for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		(*it)->Update(_window);
	}

	//run all simulation of compute shaders
	list<IComputeShader*>::iterator its;
	for (its = m_computes.begin(); its != m_computes.end(); its++)
	{
		(*its)->Simulate();
	}

	//toggle wireframe mode
	static int prevSpaceState = GLFW_RELEASE;
	if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS && prevSpaceState != GLFW_PRESS)
	{
		ToggleWireMode();
	}
	prevSpaceState = glfwGetKey(_window, GLFW_KEY_F);
}

/////////////////////////////////////////////////////////////////////////////////////
// Init() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::Init(float _w, float _h, Game* _game)
{
	m_width = _w;
	m_height = _h;

	m_game = _game;

	//load all data from the scene.sdf file
	OpenSDF();

	//load and compile all shaders
	//also creates the driver objects for compute shaders
	glBindVertexArray(0);
	m_ShaderInfo.Create(m_shader_count, ShaderTable, &m_computes);

	//initialise all cameras
	//game is passed down here to allow for linking of cameras to game objects
	//this scene is passed down to allow linking to other render objects
	bool setUseCamera = false;
	for (list<cCamera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		(*it)->Init(_w, _h, _game, this);

		//if a camera is called MAIN
		//this will be the starting camera used
		if ((*it)->GetName() == "MAIN")
		{
			m_useCamera = it;
			setUseCamera = true;
		}
		(*it)->SetPrim(FindPrim((*it)->GetPrimType()));
	}

	//if no MAIN camera just find a camera we can use
	if (!setUseCamera)
	{
		m_useCamera = m_Cameras.begin();
		NextCamera();
	}

	//give each light its primitive
	for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); it++)
	{
		(*it)->SetPrim(FindPrim((*it)->GetPrimType()));
	}

	//set up my cueb maps
	for (int i = 0; i < m_cubeMap_count; i++)
	{
		m_CubeMaps[i].Init();
	}

#ifdef _DEBUG
	// load in the primitives here
	for (int i = 0; i < m_prim_count; i++)
	{
		m_Prims[i].Init(&m_Cameras, &m_computes, &m_ShaderInfo);
	}

	for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); it++)
	{
		// set up pointers to the mesh we wish to use to represent this light
		(*it)->SetPrim(FindPrim((*it)->GetPrimType()));
	}
#endif

	//initialise all meshes, links them to appropriate shaders and textures
	for (int i = 0; i < m_mesh_count; i++)
	{
		m_Meshes[i].Init(&m_Cameras, &m_computes, &m_ShaderInfo);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// render() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::Render()
{
	if (m_wireMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//this is a base render pass system where each Render Pass is associated with a camera
	list<cCamera*>::iterator it;
	for (it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		//skip current camera using or cameras that don't draw to a Render Target
		if (it != m_useCamera && !(*it)->CanBeMain())
		{
			RenderLoop((*it));
		}
	}
	//now render current camera
	RenderLoop((*m_useCamera));

	glBindVertexArray(0);
}

//pass suitable data over to the shaders
void cScene::SetGlobalValues(unsigned int _prog)
{
	//only certain other cameras need to pass information on to all other cameras
	// say the cameraassociated with shadow mapping
	list<cCamera*>::iterator it;
	for (it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		(*it)->SetGlobalRenderValues(_prog);
	}

	//everything needs to knwo about all the lights
	for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); it++)
	{
		(*it)->SetRenderValues(_prog);
	}

	for (int i = 0; i <= m_cubeMap_count; i++)
	{
		// set cubempas to bings 4,5,6,7
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMaps[i].m_Cubemap);

		//curremtly don;t really need to do this EVERY frame
		//but of you do somethign clever with them you might need this.
	}


}

/////////////////////////////////////////////////////////////////////////////////////
// openSDF() - Opens up a scene descriptor file
/////////////////////////////////////////////////////////////////////////////////////
void cScene::OpenSDF()
{
	// load the scene file here

	FILE* fp = NULL;

	fopen_s(&fp, "scene.sdf", "r");

	if (!fp)
		assert(0);

	char buffer[256] = { "\0" };
	float value;

	////////////////////////////////////////////////////////////
	// CAMERAS
	////////////////////////////////////////////////////////////
	fscanf_s(fp, "%s%d", buffer, 256, &m_camera_count);		// CAMERA_COUNT: 2

	printf("CAMERA_COUNT: %d\n", m_camera_count);

	for (int i = 0; i < m_camera_count; i++)
	{
		char name[256] = { "\0" };
		char type[256] = { "\0" };

		fscanf_s(fp, "%s%s", buffer, 256, name, 256);	//CAMERA: name
		fscanf_s(fp, "%s", buffer, 256);					// }
		printf("CAMERA: %s\n{\n", name);

		fscanf_s(fp, "%s%s", buffer, 256, type, 256);		//TYPE: CAMERA
		printf("TYPE: %s\n", type);

		cCamera* newCam = CameraFactory::makeNewCam(type);
		newCam->SetName((string)name);
		newCam->Load(fp);

		fscanf_s(fp, "%s", buffer, 256);				// }
		printf("}\n");

		if (newCam->PushBack())
		{
			m_Cameras.push_back(newCam);
		}
		else
		{
			m_Cameras.push_front(newCam);
		}
	}

	////////////////////////////////////////////////////////////
	// LIGHTS
	////////////////////////////////////////////////////////////


	fscanf_s(fp, "%s%d", buffer, 256, &m_light_count);		// LIGHT_COUNT: 2
	printf("\n\n\nLIGHT_COUNT: %d\n", m_light_count);

	for (int i = 0; i < m_light_count; i++)
	{
		char name[256] = { "\0" };
		char type[256] = { "\0" };

		fscanf_s(fp, "%s%s", buffer, 256, name, 256);	//LIGHT: name
		fscanf_s(fp, "%s", buffer, 256);					// }
		printf("LIGHT: %s\n{\n", name);

		fscanf_s(fp, "%s%s", buffer, 256, type, 256);		//TYPE: LIGHT
		printf("TYPE: %s\n", type);

		cLight* newLight = LightFactory::makeNewLight((string)type);
		newLight->SetName((string)name);
		newLight->Load(fp);

		fscanf_s(fp, "%s", buffer, 256);				// }
		printf("}\n");

		m_Lights.push_back(newLight);
	}

	////////////////////////////////////////////////////////////
	// Cube Maps
	////////////////////////////////////////////////////////////

	fscanf_s(fp, "%s%d", buffer, 256, &m_cubeMap_count);		// CUBE_MAPS: 1
	printf("\n\n\CUBE_MAPS: %d\n", m_cubeMap_count);

	fscanf_s(fp, "%s", buffer, 256);				// {

	if (m_cubeMap_count > 4)
	{
		printf("CUBE_MAPS : %d MAXIMUM CUBEMAPS == 4!\n", m_cubeMap_count);
		assert(0);
	}
	m_CubeMaps = new cCubeMap[m_cubeMap_count];

	// check if we are trying to load a skybox
	for (int i = 0; i < m_cubeMap_count; i++)
	{
		// allocate block of memory to store the 6 x textures needed for the skybox
		m_CubeMaps[i].m_tex = (char**)malloc(6 * sizeof(char*));
		char path[64];
		for (int j = 0; j < 6; j++)
		{
			strcpy_s(path, "tga/");
			// load all six texture paths
			fscanf_s(fp, "%s%s", buffer, 256, buffer, 256);
			strcat_s(path, buffer);
			int sz = strlen(path) + 1;
			m_CubeMaps[i].m_tex[j] = (char*)malloc(sz * sizeof(char));
			strcpy_s(m_CubeMaps[i].m_tex[j], sizeof(char) * sz, path);
		}
	}

	fscanf_s(fp, "%s", buffer, 256);				// }

	////////////////////////////////////////////////////////////
	// PRIMITIVES
	////////////////////////////////////////////////////////////
	fscanf_s(fp, "%s%d", buffer, 256, &m_prim_count);		// PRIM_COUNT: 1
	printf("\n\n\PRIM_COUNT: %d\n", m_prim_count);

	// allocate memory to store the basic primitive types needed to display lights 
	m_Prims = new cMesh[m_prim_count];

	for (int i = 0; i < m_prim_count; i++)
	{
		m_Prims[i].PrimLoad(fp);
	}

	////////////////////////////////////////////////////////////
	// MESHES
	////////////////////////////////////////////////////////////
	fscanf_s(fp, "%s%d", buffer, 256, &m_mesh_count);		// MESH_COUNT: 4
	printf("\n\n\MESH_COUNT: %d\n", m_mesh_count);

	// allocate block of memory to store the mesh objects..
	m_Meshes = new cMesh[m_mesh_count];

	for (int i = 0; i < m_mesh_count; i++)
	{
		m_Meshes[i].Load(fp);
	}

	////////////////////////////////////////////////////////////
	// Game Objects
	////////////////////////////////////////////////////////////
	m_game->OpenSDF(fp, this);


	////////////////////////////////////////////////////////////
	// SHADERS
	////////////////////////////////////////////////////////////
	fscanf_s(fp, "%s%d", buffer, 256, &m_shader_count);		// SHADER_COUNT: 2
	printf("\n\n\SHADER_COUNT: %d\n", m_shader_count);

	ShaderTable = new sShaderDetails[m_shader_count];

	fscanf_s(fp, "%s", buffer, 256); // SHADER:

	for (int i = 0; i < m_shader_count; i++)
	{
		// Init shader types..
		for (int j = 0; j < 6; j++)
			ShaderTable[i].types[j] = 0;


		fscanf_s(fp, "%s", buffer, 256);
		strcpy_s(ShaderTable[i].filename, buffer);

		int loop_count = 0;

		while (strcmp(buffer, "SHADER:") != 0)
		{
			loop_count++;

			fscanf_s(fp, "%s", buffer, 256);
			if (strcmp(buffer, "SHADER:") == 0)
				break;

			// sort the shader by type and add to List..
			if (strcmp(buffer, "VERT_SHDR") == 0)
				ShaderTable[i].types[0] = GL_VERTEX_SHADER;
			else
				if (strcmp(buffer, "CTRL_SHDR") == 0)
					ShaderTable[i].types[1] = GL_TESS_CONTROL_SHADER;
				else
					if (strcmp(buffer, "EVAL_SHDR") == 0)
						ShaderTable[i].types[2] = GL_TESS_EVALUATION_SHADER;
					else
						if (strcmp(buffer, "GEOM_SHDR") == 0)
							ShaderTable[i].types[3] = GL_GEOMETRY_SHADER;
						else
							if (strcmp(buffer, "FRAG_SHDR") == 0)
								ShaderTable[i].types[4] = GL_FRAGMENT_SHADER;
							else
								ShaderTable[i].types[5] = GL_COMPUTE_SHADER;

			if (loop_count == 6)
				break;
		};
	}

	fclose(fp);

}

cMesh* cScene::FindMesh(string _name)
{
	cMesh* found = nullptr;
	for (int i = 0; i < this->m_mesh_count; i++)
	{
		if (_name == m_Meshes[i].GetName())
		{
			found = &m_Meshes[i];
			return found;
		}
	}
	printf("UNKOWN MESH : %s\n", _name.c_str());
	assert(found);
	return found;
}

cLight* cScene::FindLight(string _light)
{
	cLight* found = nullptr;
	for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); it++)
	{
		if (_light == (*it)->GetName());
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN LIGHT NAME : %s\n", _light.c_str());
	assert(found);
	return found;
}

cMesh* cScene::FindPrim(string _prim)
{
	//we don't want a prim for this
	if (_prim == "")
	{
		return nullptr;
	}

	cMesh* found = nullptr;
	for (int i = 0; i < this->m_prim_count; i++)
	{
		if (_prim == m_Prims[i].GetName())
		{
			found = &m_Prims[i];
			return found;
		}
	}
	printf("UNKOWN PRIM : %s\n", _prim.c_str());
	assert(found);
	return found;
}

cCamera* cScene::FindCam(string _cam)
{
	cCamera* found = nullptr;
	for (list<cCamera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); it++)
	{
		if (_cam == (*it)->GetName());
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN CAMERA NAME : %s\n", _cam.c_str());
	assert(found);
	return found;
}

void cScene::NextCamera()
{
	do
	{
		++m_useCamera;
		if (m_useCamera == m_Cameras.end())
			m_useCamera = m_Cameras.begin();
	} while (!(*m_useCamera)->CanBeMain());
}

void cScene::RenderLoop(cCamera* _useCam)
{
	glViewport(0, 0, m_width, m_height);

	bool depthPass = _useCam->RenderSetup();

	//don't like this over there but this allows multiple use of meshes
	m_game->RenderMeshes(_useCam, this);

#ifdef _DEBUG
	if (!depthPass)
	{
		for (list<cLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); it++)
		{
			// render a primitive mesh object to represent this light source
			(*it)->PrepPrimRender();
			(*it)->GetPrim()->Render(_useCam, this, (*it)->GetCol());
		}

		for (list<cCamera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
		{
			//ditto but for the cameras
			//don't do this if doesn't have a prim or its the current camera
			if ((*it)->GetPrim() && _useCam != (*it))
			{
				(*it)->PrepPrimRender();
				(*it)->GetPrim()->Render(_useCam, this);
			}
		}
	}
#endif
	if (depthPass)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
