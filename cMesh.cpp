#include "cMesh.h"
#include <string.h>
#include <stdio.h>
#include "core.h"
#include "cScene.h"
#include "cCamera.h"
#include "cTGAImg.h"
#include "IComputeShader.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "helper.h"
#include "cTexture.h"

#include <string>

using namespace glm;

using std::string;

const unsigned int vertexPos_loc = 0;   // Corresponds to "location = 0" in the vertex shader definition
const unsigned int texCoord_loc = 1;
const unsigned int vertexNormal_loc = 2;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// cMesh() - 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
cMesh::cMesh()
{
	m_fp = 0;

	m_e_count = 0;
	m_v_count = 0;
	m_vt_count = 0;
	m_vn_count = 0;
	m_f_count = 0;

	m_v = NULL;
	m_vt = NULL;
	m_vn = NULL;
	m_fn = NULL;
	m_f = NULL;

	m_vList = NULL;
	m_eList = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~cMesh() - 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
cMesh::~cMesh()
{
	if (m_v)
		delete[] m_v;
	if (m_vt)
		delete[] m_vt;
	if (m_vn)
		delete m_vn;
	if (m_f)
		delete m_f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init() - initialise a mesh, links them to appropriate shaders and textures
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::Init(list<cCamera*>* _cams, list<IComputeShader*>* _comps, cShaderInfo* _ShaderInfo)
{
	Load("Meshes/", m_filename.c_str());

	m_shProg = _ShaderInfo->GetSPbyName(m_shaderName);
	m_hasTess = _ShaderInfo->HasTessByName(m_shaderName);

	// generate the number of textures we need for this number of materials
	//HACK: but what if we have multiple textures for a material
	glGenTextures(m_material_count, m_tex);

	for (int i = 0; i < m_material_count; i++)
	{
		string textureFile = MatList(i)->tex[0];
		string textureName = textureFile.substr(4, textureFile.size() - 8);
		string textureType = textureFile.substr(textureFile.size() - 3);

		//this allows us to link to the render targets of custom cameras
		if (textureType == "cam" || textureType == "ref")
		{
			list<cCamera*>::iterator it;
			for (it = _cams->begin(); it != _cams->end(); ++it)
			{
				if (textureName == (*it)->GetName())
				{
					m_tex[i] = (*it)->GetRT();
				}
			}
		}//this is for linking to the output texture of compute shaders
		else if (textureType == "cmp") //give the pointer for this to compute shader to populate
		{
			list<IComputeShader*>::iterator its;
			for (its = _comps->begin(); its != _comps->end(); its++)
			{
				if (textureName == (*its)->GetName())
				{
					(*its)->AddUsedTex(&m_tex[i]);
				}
			}
		}
		else
		{
			//else its a TGA that we need to load
			cTexture::LoadTGA(MatList(i)->tex[0], m_tex[i], i);
			// bind the flag texture..
			glBindTextureUnit(i, m_tex[i]);
		}
	}

	//actually create the vertex buffer type objects to be able to pass to the GPU
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cVertex) * 3 * m_f_count, m_vList, GL_STATIC_DRAW); // triangles
	glVertexAttribPointer(vertexPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)0);
	glEnableVertexAttribArray(vertexPos_loc);
	glVertexAttribPointer(vertexNormal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vertexNormal_loc);
	glVertexAttribPointer(texCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(texCoord_loc);


	// lock down this data to the current VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// render() - 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::Render(class cCamera* _cam, cScene* _scene, vec3 _useCol)
{
	if (m_name == _cam->GetName())
		return;//don't draw me in my own reflection

	// this is the shader program we're using..
	unsigned int s_prog = _cam->HasCustomShader() ? _cam->GetCustomShader() : m_shProg;
	bool hasTess = _cam->HasCustomShader() ? _cam->HasCustomShaderTess() : m_hasTess;

	// bind the required textures for this mesh
	for (int i = 0; i < m_material_count; i++)
	{
		glBindTextureUnit(i, m_tex[i]);
	}

	// enable shader program..
	glUseProgram(s_prog);

	GLint loc;

	if (Helper::SetUniformLocation(s_prog, "modelMatrix", &loc))
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_worldMatrix));

	_cam->SetRenderValues(s_prog);
	_scene->SetGlobalValues(s_prog);

	if (Helper::SetUniformLocation(s_prog, "useCol", &loc))
		glUniform3fv(loc, 1, glm::value_ptr(_useCol));

	// Draw FACES
	int vertexCount = m_f_count * 3; // calculate the number of vertices for this object
	glBindVertexArray(m_VAO);

	//if we are using a tesselation using GL_PATCHES
	//HACK: if we are using more advanced tesselation than just the triangle domain here
	//there are some other things we'll need to set as well (and they'll need to match what's in the shader)
	//see the tesselation shader lecture notes
	if (!hasTess)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	else
	{
		glDrawArrays(GL_PATCHES, 0, vertexCount);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// createFaceNormalList() - load into memory List of face normals
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::CreateFaceNormalList()
{
	// allocate memory to hold one face normal per face
	m_fn = new cVec3f[(int)m_f_count];

	// we don't have any normals in this .obj file
	if (m_vn_count == 0)
	{
		// allocate the memory needed to Create and store the face normals
		// set the vertex normal count to be the same as the face count
		m_vn_count = m_f_count;
		m_vn = new cVec3f[m_vn_count];

		// loop through each face and calculate the face normal
		for (int f = 0; f < m_f_count; f++)
		{
			// first we need to generate the vertex normals... cos this .obj file doesn't have them - doh!

			cVec3f v1 = m_v[m_f[f].v_id[1]] - m_v[m_f[f].v_id[0]];
			cVec3f v2 = m_v[m_f[f].v_id[2]] - m_v[m_f[f].v_id[0]];
			vec3 v3 = cross(vec3(v1.x, v1.y, v1.z), vec3(v2.x, v2.y, v2.z));

			for (int i = 0; i < 3; i++)
			{
				m_vn[m_f[f].vn_id[i]].x = v3.x;
				m_vn[m_f[f].vn_id[i]].y = v3.y;
				m_vn[m_f[f].vn_id[i]].z = v3.z;

				m_vn[m_f[f].vn_id[i]].normalize();
			}
			m_fn[f] = (m_vn[m_f[f].vn_id[0]] + m_vn[m_f[f].vn_id[1]] + m_vn[m_f[f].vn_id[2]]);

			m_fn[f].normalize();
		}
	}
	else
	{
		// loop through each face and calculate the face normal
		for (int f = 0; f < m_f_count; f++)
		{
			m_fn[f] = (m_vn[m_f[f].vn_id[0]] + m_vn[m_f[f].vn_id[1]] + m_vn[m_f[f].vn_id[2]]);

			m_fn[f].normalize();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// faceList() - load into memory List of faces
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::CreateFaceList()
{
	m_vList = new cVertex[((int)m_f_count) * 3];

	for (int f = 0; f < m_f_count; f++)
	{
		// vertices
		m_vList[f * 3 + 0].m_v = m_v[m_f[f].v_id[0]];
		m_vList[f * 3 + 1].m_v = m_v[m_f[f].v_id[1]];
		m_vList[f * 3 + 2].m_v = m_v[m_f[f].v_id[2]];

		// vertex normals
		m_vList[f * 3 + 0].m_n = m_vn[m_f[f].vn_id[0]];
		m_vList[f * 3 + 1].m_n = m_vn[m_f[f].vn_id[1]];
		m_vList[f * 3 + 2].m_n = m_vn[m_f[f].vn_id[2]];

		// texture coordinates
		m_vList[f * 3 + 0].m_uv = m_vt[m_f[f].vt_id[0]];
		m_vList[f * 3 + 1].m_uv = m_vt[m_f[f].vt_id[1]];
		m_vList[f * 3 + 2].m_uv = m_vt[m_f[f].vt_id[2]];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CreateEdgeList() - load into memory List of edges
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::CreateEdgeList()
{
	m_e_count = m_f_count * 3;
	int num_vertices = m_e_count * 2;
	m_eList = new cVec3f[num_vertices]; // two vertices needed to per edge

	int e = 0;
	for (int f = 0; f < m_f_count; f++)
	{
		m_eList[e++] = m_v[m_f[f].v_id[0]];
		m_eList[e++] = m_v[m_f[f].v_id[1]];

		m_eList[e++] = m_v[m_f[f].v_id[1]];
		m_eList[e++] = m_v[m_f[f].v_id[2]];

		m_eList[e++] = m_v[m_f[f].v_id[2]];
		m_eList[e++] = m_v[m_f[f].v_id[0]];
	}
}

void cMesh::PrimLoad(FILE* _fp)
{
	char buffer[256] = { "\0" };
	char name[256] = { "\0" };
	char fileName[256] = { "\0" };

	fscanf_s(_fp, "%s%s", buffer, 256, name, 256);	//PRIM: cube
	m_name = name;
	fscanf_s(_fp, "%s", buffer, 256);	// }
	printf("PRIM: %s\n{\n", name);

	fscanf_s(_fp, "%s%s", buffer, 256, fileName, 256); // FILENAME:	cube.obj
	printf("FILENAME: %s\n", fileName);

	m_filename = fileName;
	m_shaderName = "flatPrim";

	fscanf_s(_fp, "%s", buffer, 256);					// }
	printf("}\n");
}

void cMesh::Load(FILE* _fp)
{
	char buffer[256] = { "\0" };
	char name[256] = { "\0" };
	char fileName[256] = { "\0" };
	char shaderName[256] = { "\0" };

	fscanf_s(_fp, "%s%s", buffer, 256, name, 256);	//MESH: cube
	m_name = name;
	fscanf_s(_fp, "%s", buffer, 256);	// }
	printf("MESH: %s\n{\n", name);

	fscanf_s(_fp, "%s%s", buffer, 256, fileName, 256); // FILENAME:	cube.obj
	m_filename = fileName;
	printf("FILENAME: %s\n", fileName);

	fscanf_s(_fp, "%s%s", buffer, 256, shaderName, 256); // SHADER: flat
	m_shaderName = shaderName;
	printf("SHADER: %s\n", shaderName);

	fscanf_s(_fp, "%s", buffer, 256);					// }
	printf("}\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// importMTL() - 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::ImportMTL()
{
	char buffer[64];
	char mtl_path[256];

	int buffer_size = sizeof(buffer);

	strcpy_s(mtl_path, m_mtl_filename);

	// load .mtl file
	fopen_s(&m_mtl, mtl_path, "r");

	if (!m_mtl)
		return;


	m_material_count = 0;

	// search through mtl file and find out how many materials we have
	do
	{
		fscanf_s(m_mtl, "%s", buffer, buffer_size);

		if (strcmp(buffer, "newmtl") == 0)
			m_material_count++;

	} while (!feof(m_mtl));

	//Just to limits things a tad, but 4 should be more than enough!

	if (m_material_count > 4)
	{
		m_material_count = 4;

		printf("RENDERER LIMIT OF FOUR MATERIALS PER MESH, REDUCING TO 4.\n");
	}

	// allocate sufficient memory to load all materials
	m_mtlList = new newmtl[m_material_count];

	fseek(m_mtl, 0, SEEK_SET);

	int count = -1;

	// search through mtl file and extract all materials for this object
	do
	{
		fscanf_s(m_mtl, "%s", buffer, buffer_size);

		if (strcmp(buffer, "newmtl") == 0)
		{
			count++;
			// read in material name
			fscanf_s(m_mtl, "%s", buffer, buffer_size);
			strcpy_s(m_mtlList[count].name, buffer);
		}
		else
		{
			if (strcmp(buffer, "Kd") == 0)
			{
				// read in diffuse values
				fscanf_s(m_mtl, "%f%f%f", &m_mtlList[count].diffuse.r, &m_mtlList[count].diffuse.g, &m_mtlList[count].diffuse.b);
			}
			else
			{
				if (strcmp(buffer, "map_Kd") == 0) // diffuse mapd
				{
					char texturePath[128];
					strcpy_s(texturePath, "tga/");
					// read in the texture name
					fscanf_s(m_mtl, "%s", buffer, (unsigned int)sizeof(buffer));
					strcat_s(texturePath, buffer);
					strcpy_s(m_mtlList[count].tex[0], texturePath);

					m_mtlList[count].m_tex_count++;

					if (m_import_filetype == FILE_TYPE_BLENDER)
					{
						break;
					}
				}
				/*
				THIS IS RATHER A HACK AS WE SHOULD BE DOING SOMETHING LIKE THIS
				INSTEAD WE SHALL JUST LOAD IN THE DIFFUSE TEXTURES FROM EACH MATERIAL
								if (strcmp(buffer, "map_Ks") == 0) // specular map
								{
									char texturePath[128];
									strcpy_s(texturePath, "tga/");
									// read in the texture name
									fscanf_s(m_mtl, "%s", buffer, (unsigned int)sizeof(buffer));
									strcat_s(texturePath, buffer);
									strcpy_s(m_mtlList[count].tex[1], texturePath);

									m_mtlList[count].m_tex_count++;

									if (m_import_filetype == FILE_TYPE_BLENDER)
									{
										break;
									}
								}
								if (strcmp(buffer, "bump") == 0) // normal map
								{
									char texturePath[128];
									strcpy_s(texturePath, "tga/");
									// read in the texture name
									fscanf_s(m_mtl, "%s%f", buffer, (unsigned int)sizeof(buffer), &m_mtlList[count].m_bump_height);
									fscanf_s(m_mtl, "%s", buffer, (unsigned int)sizeof(buffer));
									strcat_s(texturePath, buffer);
									strcpy_s(m_mtlList[count].tex[2], texturePath);

									m_mtlList[count].m_tex_count++;

									if (m_import_filetype == FILE_TYPE_BLENDER)
									{
										break;
									}
								}
								*/
			}
		}

	} while (!feof(m_mtl));


	// OK, so we've finished with the .mtl file
	fclose(m_mtl);
	m_mtl = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// loadFaces() - extracts face data from .obj file
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::LoadFaces()
{
	char buffer[32] = { NULL };
	int  f[3] = { NULL };
	char slash[2] = { NULL };
	int  face = 0;
	int  m_index = 0;
	char materialName[64] = { NULL };

	fseek(m_fp, 0, SEEK_SET);

	do
	{
		fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
	} while (strcmp(buffer, "usemtl") != 0 && strcmp(buffer, "#usemtl") != 0);
	fscanf_s(m_fp, "%s", materialName, (unsigned int)sizeof(materialName));// material name

	// convert material name into index into m_mtlList
	for (int m = 0; m < m_material_count; m++)
	{
		if (strcmp(materialName, m_mtlList[m].name) == 0)
		{
			m_index = m;
			break;
		}
	}


	// search for the first face
	while (strcmp(buffer, "f") != 0)
		fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));

	do
	{
		for (int i = 0; i < 3; i++)
		{
			if (m_vn_count > 0)
			{
				fscanf_s(m_fp, "%d%c%d%c%d", &f[0], &slash, (unsigned int)sizeof(slash), &f[1], &slash, (unsigned int)sizeof(slash), &f[2]);
			}
			else
			{
				fscanf_s(m_fp, "%d%c%d", &f[0], &slash, (unsigned int)sizeof(slash), &f[1]);
				f[2] = face;
			}

			// store indices for each vertex on this face here..
			if (i == 0)
			{
				m_f[face].mtl_id = m_index;
			}

			if (m_import_filetype == FILE_TYPE_BLENDER)
			{
				m_f[face].v_id[i] = f[0] - 1;
				m_f[face].vt_id[i] = f[1] - 1;
				m_f[face].vn_id[i] = f[2] - 1;
			}
			else
			{
				m_f[face].v_id[i] = f[0] - 1;
				m_f[face].vt_id[i] = f[1] - 1;
				m_f[face].vn_id[i] = f[2] - 1;
			}
		}
		face++;

		// OK, so we've finished loading all the faces in this file
		if (face == m_f_count)
			return;

		fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));

		// step over surface flag
		if (strcmp(buffer, "s") == 0)
			fscanf_s(m_fp, "%s%s", buffer, unsigned int(sizeof(char) * 32), buffer, unsigned int(sizeof(char) * 32));

		if (strcmp(buffer, "g") == 0)
		{
			// search for another material
			do
			{
				fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
			} while (strcmp(buffer, "usemtl") != 0);
			fscanf_s(m_fp, "%s", materialName, (unsigned int)sizeof(materialName));// material name

			// convert material name into index into m_mtlList
			for (int m = 0; m < m_material_count; m++)
			{
				if (strcmp(materialName, m_mtlList[m].name) == 0)
				{
					m_index = m;
					break;
				}
			}
			fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
		}
	} while (strcmp(buffer, "f") == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// load() - 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cMesh::Load(const char* _obj_path, const char* _obj_name)
{
	char filepath[64];

	strcpy_s(filepath, _obj_path);
	strcat_s(filepath, _obj_name);

	fopen_s(&m_fp, filepath, "r");

	if (!m_fp)
		return;

	static char buffer[32];

	// initialise the import type to use MAYA .obj
	m_import_filetype = FILE_TYPE_MAYA;

	// find the material library used for this object..
	do
	{
		fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));

		if (strcmp(buffer, "3ds") == 0)
		{
			m_import_filetype = FILE_TYPE_3DSMAX;
		}

		if (strcmp(buffer, "Blender") == 0)
		{
			m_import_filetype = FILE_TYPE_BLENDER;
		}
	} while (strcmp(buffer, "mtllib") != 0);

	// this is the .mtl filename
	fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));

	// store .mtl filename for later use
	strcpy_s(m_mtl_filename, _obj_path);
	strcat_s(m_mtl_filename, buffer);

	char meshName[64];

	// read ahead and find out how much data we have
	do
	{
		fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));

		// If we change from "f" to "v" then we have another mesh object in this file...
		// We need to start counters again for the next mesh object
		if (strcmp(buffer, "v") == 0)m_v_count++;
		if (strcmp(buffer, "vt") == 0)m_vt_count++;
		if (strcmp(buffer, "vn") == 0)m_vn_count++;
		if (strcmp(buffer, "f") == 0)m_f_count++;

	} while (!feof(m_fp));


	// allocate memory required to hold the data
	m_v = new cVec3f[m_v_count];
	m_vt = new sVec2f[m_vt_count];
	if (m_vn_count > 0)
	{
		m_vn = new cVec3f[m_vn_count];
	}

	m_f = new sFace[m_f_count];

	fseek(m_fp, 0, SEEK_SET);

	// vertices
	for (int i = 0; i < m_v_count; i++)
	{
		do
		{
			fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
		} while (strcmp(buffer, "v") != 0);

		fscanf_s(m_fp, "%f%f%f", &m_v[i].x, &m_v[i].y, &m_v[i].z);
	}

	fseek(m_fp, 0, SEEK_SET);

	if (m_import_filetype == FILE_TYPE_3DSMAX)
	{
		// ensure the mesh has been exported with normals. If it hasn't,
		// we'll need to generate them ourselves
		if (m_vn_count > 0)
		{
			// vertex normals
			for (int i = 0; i < m_vn_count; i++)
			{
				do
				{
					fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
				} while (strcmp(buffer, "vn") != 0);
				fscanf_s(m_fp, "%f%f%f", &m_vn[i].x, &m_vn[i].y, &m_vn[i].z);
			}

			fseek(m_fp, 0, SEEK_SET);
		}
		float z_coord; // FILE_TYPE_3DSMAX supports 3d texture coords... the z is not being stored

		// texture coordinates
		for (int i = 0; i < m_vt_count; i++)
		{
			do
			{
				fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
			} while (strcmp(buffer, "vt") != 0);
			fscanf_s(m_fp, "%f%f%f", &m_vt[i].x, &m_vt[i].y, &z_coord);
		}
	}
	else
	{
		// texture coordinates
		for (int i = 0; i < m_vt_count; i++)
		{
			do
			{
				fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
			} while (strcmp(buffer, "vt") != 0);
			fscanf_s(m_fp, "%f%f", &m_vt[i].x, &m_vt[i].y);
		}

		// return back to start of file ready for the big read
		fseek(m_fp, 0, SEEK_SET);

		// again, we're checking that the mesh has been exported with its own normals
		if (m_vn_count > 0)
		{
			// vertex normals
			for (int i = 0; i < m_vn_count; i++)
			{
				do
				{
					fscanf_s(m_fp, "%s", buffer, unsigned int(sizeof(char) * 32));
				} while (strcmp(buffer, "vn") != 0);
				fscanf_s(m_fp, "%f%f%f", &m_vn[i].x, &m_vn[i].y, &m_vn[i].z);
			}
		}
	}



	// load in all material data...
	ImportMTL();

	// load in face data
	LoadFaces();

	CreateFaceNormalList();
	CreateFaceList();

	fclose(m_fp);
	m_fp = NULL;
}