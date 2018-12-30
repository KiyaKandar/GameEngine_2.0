#include "TableCreation.h"
#include "Table.h"
#include "../Resources/Resource.h"
#include "../../Graphics/Meshes/Mesh.h"
#include "../../Gameplay/GameObject.h"
#include "../../Graphics/Scene Management/SceneNode.h"
#include "../../Graphics/Utility/Light.h"
#include "../../Audio/Sound.h"
#include "../GameObjectBuilder.h"

const size_t MAX_MEMORY_PER_TABLE = 50000;

TableCreation::TableCreation(Database* database)
{
	this->database = database;

	tableAdditions.push_back(std::bind(&TableCreation::AddGameObjectsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddMeshesTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddSoundsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddLightsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddUiMeshTable, this));

	AddTablesToDatabase();}

TableCreation::~TableCreation()
{
}

void TableCreation::AddTablesToDatabase() const
{
	for (const auto addTableToDatabase : tableAdditions)
	{
		addTableToDatabase();
	}
}

void TableCreation::AddGameObjectsTable() const
{
	database->AddTable("GameObjects", new Table<Resource>("GameObjects", MAX_MEMORY_PER_TABLE, [&database = database](Node* node)
	{
		return GameObjectBuilder::BuildGameObject(node, database);	
	}));
}


void TableCreation::AddMeshesTable() const
{
	database->AddTable("Meshes", new Table<Resource>("Meshes", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Mesh* mesh;

		if (node->children[0]->nodeType.compare("MeshType") == 0)
		{
			mesh = Mesh::GenerateHeightMap(30,30);
			mesh->SetName(node->name);
			mesh->LoadTexture(node->children[1]->value);
			mesh->perlin = stoi(node->children[2]->value);
		}
		else
		{
			mesh = new Mesh(node->children[0]->value, 1, node->name);

			if (node->children.size() > 1)
			{
				mesh->SetTextureFile(node->children[1]->value);
			}
		}

		return mesh;
	}));
}

void TableCreation::AddUiMeshTable() const
{
	database->AddTable("UIMeshes", new Table<Resource>("UIMeshes", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Mesh* mesh = new Mesh(node->children[0]->value, 1);
		mesh->SetName(node->name);
		return mesh;
	}));
}

void TableCreation::AddSoundsTable() const
{
	database->AddTable("SoundObjects", new Table<Resource>("SoundObjects", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Sound *sound = new Sound(node->value);
		sound->SetName(node->name);
		return sound;
	}));
}

void TableCreation::AddLightsTable() const
{
	database->AddTable("Lights", new Table<Resource>("Lights", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		std::string resourceName = node->name;

		Node* positionNode = node->children[0];
		NCLVector3 position(
			std::stof(positionNode->children[0]->value),
			std::stof(positionNode->children[1]->value),
			std::stof(positionNode->children[2]->value));

		Node* colourNode = node->children[1];
		NCLVector4 colour(
			std::stof(colourNode->children[0]->value),
			std::stof(colourNode->children[1]->value),
			std::stof(colourNode->children[2]->value),
			std::stof(colourNode->children[3]->value));

		float radius = std::stof(node->children[2]->value);
		float intensity = std::stof(node->children[3]->value);

		bool isShadowCasting = (node->children[4]->value == "enabled");

		Light* light = new Light(position, colour, radius, intensity, isShadowCasting);
		light->SetName(resourceName);

		return light;
	}));
}
