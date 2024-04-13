#include "Game.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "cScene.h"
#include "cMesh.h"

Game::Game()
{
}

//tick all my Game Objects
//the window is being passed in to allow things liek keyboard control of your player character
void Game::Update(GLFWwindow* _window)
{
	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(_window);
	}
}

void Game::AddGameObject(GameObject* _new)
{
	m_GameObjects.push_back(_new);
}

//I want THAT Game Object by name
GameObject* Game::GetGameObject(string _GOName)
{
	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if ((*it)->GetName() == _GOName)
		{
			return (*it);
		}
	}
	printf("Unknown Game Object NAME : %s \n", _GOName.c_str());
	assert(0);
	return nullptr;
}

void Game::OpenSDF(FILE* _fp, cScene* _scene)
{
	char buffer[256] = { "\0" };
	int GO_count;

	fscanf_s(_fp, "%s%d", buffer, 256, &GO_count); //GAME_OBJECT_COUNT: 11
	printf("\n\n\GAME_OBJECT_COUNT: %d\n", GO_count);

	for (int i = 0; i < GO_count; i++)
	{
		//Load data about each Game Object from the SDF file
		char name[256] = { "\0" };
		char type[256] = { "\0" };
		char mesh[256] = { "\0" };

		fscanf_s(_fp, "%s%s", buffer, 256, name, 256);	//GAME_OJECT: name
		printf("GAME_OBJECT: %s\n{\n", name);
		fscanf_s(_fp, "%s", buffer, 256);					// {
		fscanf_s(_fp, "%s%s", buffer, 256, type, 256);		//TYPE: GAME_OBJECT
		printf("TYPE: %s\n", type);
		fscanf_s(_fp, "%s%s", buffer, 256, mesh, 256);		//MESH: MESH 
		printf("MESH: %s\n", mesh);

		GameObject* newObject = GameObjectFactory::makeNewGO(type);
		//Custom features loaded by GameObject otself
		newObject->Load(_fp);

		fscanf_s(_fp, "%s", buffer, 256);					// }
		printf("}\n");

		//Set main data values
		newObject->SetName(name);
		newObject->SetMesh(_scene->FindMesh(mesh));
		m_GameObjects.push_back(newObject);
	}
}

//don't like this being here but..
void Game::RenderMeshes(class cCamera* _cam, cScene* _scene)
{
	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if ((*it)->GetMesh())
		{
			//as more than one Game Object can be using this mesh
			//set its world transform to this Game Object and Render it
			//instead of just updating that value just after I update the game object
			(*it)->GetMesh()->SetWorldMatrix((*it)->GetTransform()->m_worldMatrix);
			(*it)->GetMesh()->Render(_cam, _scene);
		}
	}
}
