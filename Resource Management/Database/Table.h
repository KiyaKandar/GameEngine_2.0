#pragma once

#include "../Resources/ResourceManager.h"
#include "../XMLParser.h"
#include "../Resources/Resource.h"

#include <string>
#include <functional>

template <class ResourceType>
class Table
{
public:
	Table(const std::string name, const size_t maxSize, std::function<ResourceType*(Node*)> builder)
	{
		this->name = name;
		this->maxSize = maxSize;
		this->builder = builder;

		storage = new ResourceManager<ResourceType>(name, maxSize);
	}

	~Table() {}

	void addNewResource(Node* resource)
	{
		storage->addResource(builder(resource));
	}

	void addNewResource(Resource* resource)
	{
		storage->addResource(resource);
	}

	void deleteResource(const std::string& identifier)
	{
		storage->deleteResource(identifier);
	}

	ResourceManager<ResourceType>* getAllResources()
	{
		return storage;
	}

	ResourceType* getResource(const std::string& identifier)
	{
		if (resourceExists(identifier))
		{
			return storage->getResource(identifier);
		}

		return nullptr;
	}

	std::string getName()
	{
		return name;
	}

	bool resourceExists(const std::string& identifier)
	{
		return storage->getResourceBuffer().find(identifier) != storage->getResourceBuffer().end();
	}


private:
	std::string name;
	size_t maxSize;

	ResourceManager<ResourceType>* storage;
	std::function<ResourceType*(Node*)> builder;
};

