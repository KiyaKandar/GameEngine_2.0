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

	tableAdditions.push_back(std::bind(&TableCreation::addGameObjectsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::addMeshesTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::addSoundsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::addLightsTable, this));
	tableAdditions.push_back(std::bind(&TableCreation::addUIMeshTable, this));

	addTablesToDatabase();}

TableCreation::~TableCreation()
{
}

void TableCreation::addTablesToDatabase() const
{
	for (const auto addTableToDatabase : tableAdditions)
	{
		addTableToDatabase();
	}
}

void TableCreation::addGameObjectsTable() const
{
	database->addTable("GameObjects", new Table<Resource>("GameObjects", MAX_MEMORY_PER_TABLE, [&database = database](Node* node)
	{
		return GameObjectBuilder::buildGameObject(node, database);	
	}));
}


void TableCreation::addMeshesTable() const
{
	database->addTable("Meshes", new Table<Resource>("Meshes", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Mesh* mesh;

		if (node->children[0]->nodeType.compare("MeshType") == 0)
		{
			mesh = Mesh::GenerateHeightMap(30,30);
			mesh->setName(node->name);
			mesh->loadTexture(node->children[1]->value);
			mesh->perlin = stoi(node->children[2]->value);
		}
		else
		{
			mesh = new Mesh(node->children[0]->value, 1, node->name);

			if (node->children.size() > 1)
			{
				mesh->setTextureFile(node->children[1]->value);
			}
		}

		return mesh;
	}));
}

void TableCreation::addUIMeshTable() const
{
	database->addTable("UIMeshes", new Table<Resource>("UIMeshes", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Mesh* mesh = new Mesh(node->children[0]->value, 1);
		mesh->setName(node->name);
		return mesh;
	}));
}

void TableCreation::addSoundsTable() const
{
	database->addTable("SoundObjects", new Table<Resource>("SoundObjects", MAX_MEMORY_PER_TABLE, [](Node* node)
	{
		Sound *sound = new Sound(node->value);
		sound->setName(node->name);
		return sound;
	}));
}

void TableCreation::addLightsTable() const
{
	database->addTable("Lights", new Table<Resource>("Lights", MAX_MEMORY_PER_TABLE, [](Node* node)
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
		light->setName(resourceName);

		return light;
	}));
}
