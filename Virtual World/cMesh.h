#pragma once

#include "core.h"
#include "cTransform.h"
#include "cShader.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

#include <list>
#include <string>
using std::string;
using std::list;

using namespace glm;

class cTransform;
class cCamera;
class IComputeShader;
class cScene;

enum eFileType
{
	FILE_TYPE_MAYA = 0,
	FILE_TYPE_3DSMAX,
	FILE_TYPE_BLENDER,
};

struct sFace
{
	int v_id[3];	// vertex indices
	int vt_id[3];	// texCoord indices
	int vn_id[3];	// normals indices
	int mtl_id;		// material index
};

class cVec3f
{
public:
	cVec3f operator +(const cVec3f _v)
	{
		cVec3f out;
		out.x = this->x + _v.x;
		out.y = this->y + _v.y;
		out.z = this->z + _v.z;

		return out;
	}

	cVec3f operator -(const cVec3f _v)
	{
		cVec3f out;
		out.x = this->x - _v.x;
		out.y = this->y - _v.y;
		out.z = this->z - _v.z;

		return out;
	}

	void normalize()
	{
		float d = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

		this->x /= d;
		this->y /= d;
		this->z /= d;
	}

	cVec3f operator /(const cVec3f _v)
	{
		cVec3f out;
		out.x = this->x / _v.x;
		out.y = this->y / _v.y;
		out.z = this->z / _v.z;

		return out;
	}

	float x, y, z;
};


struct sVec2f
{
	float x, y;
};

struct sRGB
{
	float r, g, b;
};

struct newmtl
{
	sRGB	diffuse;
	int		m_tex_count = 0;
	float	m_bump_height;
	char	tex[4][64]; // 4 x texture maps supported: colour, specular, normal, unused
	char	name[64];
};

class cFace
{
public:
	cVec3f	m_v[3];
	cVec3f	m_fn;
};

class cVertex
{
public:
	cVec3f	m_v;  // vertex
	cVec3f	m_n;  // vertex normal
	sVec2f	m_uv; // texture coordinates
};

//loads in from an obj file a mesh
//also loads its textures based on the mtl file assoicated with it
//the code for loading the mesh in from the obj file is from the original code base
//it "works" but not entirely sure how
class cMesh
{
public:
	cMesh();
	~cMesh();

	//initilise this mesh
	//these data structure might be needed to link to textures from render targets and compute shaders
	//shaderinfo allows this mesh to find its shader as given in the SDF
	void Init(list<cCamera*>* _cams, list<IComputeShader*>* _compsc, cShaderInfo* _ShaderInfo);

	void Render(class cCamera* _cam, cScene* _scene, vec3 _useCol = vec3(1, 1, 1));

	//Load the data from the obj to construct this mesh
	void Load(const char* _obj_path, const char* _obj_name);
	void ImportMTL();
	void LoadFaces();

	void CreateFaceNormalList();
	void CreateFaceList();
	void CreateEdgeList();

	inline int NumFaces()
	{
		return m_f_count;
	}
	inline cVec3f* v(int _index)
	{
		return &m_v[_index];
	}

	inline newmtl* MatList(int _index)
	{
		return &m_mtlList[_index];
	}

	//Load for a proper mesh from the scene.sdf file
	void Load(FILE* fp);
	//Load for a primitive mesh from the scene.sdf file
	void PrimLoad(FILE* fp);

	//hello my name is
	string GetName() { return m_name; }

	//I don't have a cTransform, instead my GameObject will give me a world Transform
	//so I know where to draw myself
	void SetWorldMatrix(glm::mat4 _mat) { m_worldMatrix = _mat; }

	//this is the shader I will be rendered with
	void SetShaderProgram(int _sp) { m_shProg = _sp; }
	unsigned int GetShaderProgram() { return m_shProg; }

protected:

	glm::mat4 m_worldMatrix;

	FILE* m_fp;
	FILE* m_mtl;

	string		m_filename;
	string		m_name;

	// this is the shader program we're using..
	unsigned int m_shProg = 0;
	string m_shaderName = "";
	bool m_hasTess = false; //does my shader use tesselation

	/////////////////////////////////////////////////////////////////////////////////////
	// VBO:		Vertex Buffer Object holds an array of data. 
	//			The data contained within the VBO is typically vertex, normal, colour and 
	//			texture coordinate data
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int m_VBO;
	/////////////////////////////////////////////////////////////////////////////////////
	// VAO:		Vertex Array Object holds info about how the data stored within the VBO is  
	//			formatted. The VAO holds a pointer to the start of each vertex attribute - 
	//			i.e. vertex, normal, colour, texture coordinate, along with the number of 
	//			elements for each attribute, (a vertex has 3 elements, a texture 
	//			coordinate has 2 elements), the step size (stride) needed to move to the  
	//			beginning of the next item in the List
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int m_VAO;

	//you can have up to 32 texture bound at any one time in the shaders
	unsigned int m_tex[32];

	//all the rest of this are mostly used by the system used to actually load the data into this
	//mesh from the OBJ file
	char m_mtl_filename[64];
	eFileType m_import_filetype;

	int m_objID;
	int m_v_count;
	int m_vt_count;
	int m_vn_count;
	int m_f_count;
	int m_e_count;
	int m_material_count;

	cVec3f* m_v;	// vertex List
	sVec2f* m_vt;	// uv coord List
	cVec3f* m_vn;	// vertex normal List
	cVec3f* m_fn;	// face normals List
	sFace* m_f;	// indices for each face vertex - i.e. m_f[0].x = m_v, m_f[0].y = m_vt, m_f[0].z = m_vn

	cVec3f* m_eList;		// edge List
	cVertex* m_vList;		// vertex List
	newmtl* m_mtlList;		// material List
};

