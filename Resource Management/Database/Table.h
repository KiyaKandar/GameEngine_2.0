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

	void AddNewResource(Node* resource)
	{
		storage->AddResource(builder(resource));
	}

	void AddNewResource(Resource* resource)
	{
		storage->AddResource(resource);
	}

	void DeleteResource(const std::string& identifier)
	{
		storage->DeleteResource(identifier);
	}

	ResourceManager<ResourceType>* GetAllResources()
	{
		return storage;
	}

	ResourceType* GetResource(const std::string& identifier)
	{
		if (ResourceExists(identifier))
		{
			return storage->GetResource(identifier);
		}

		return nullptr;
	}

	std::string GetName() const
	{
		return name;
	}

	bool ResourceExists(const std::string& identifier)
	{
		return storage->GetResourceBuffer().find(identifier) != storage->GetResourceBuffer().end();
	}


private:
	std::string name;
	size_t maxSize;

	ResourceManager<ResourceType>* storage;
	std::function<ResourceType*(Node*)> builder;
};

