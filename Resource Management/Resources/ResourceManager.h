#pragma once
#include <unordered_map>
#include <algorithm>

template<class T>
class ResourceManager
{
public:
	ResourceManager()
	{
		resourceManagerID = "";
		maxSize = 0;
		currentSize = 0;
	}

	ResourceManager(std::string id, size_t upperbound)
	{
		resourceManagerID = id;
		maxSize = upperbound;
		currentSize = 0;
	}

	~ResourceManager()
	{
		DeleteAllResources();
	}

	void AddResource(T* resource)
	{
		if ((currentSize + resource->GetSize()) <= maxSize)
		{
			resourceBuffer.insert({ resource->GetName(), resource });
			currentSize += resource->GetSize();
		}
	}

	T* GetResource(std::string identifier)
	{
		if (resourceBuffer.find(identifier) == resourceBuffer.end())
		{
			return nullptr;
		}
		else
		{
			return resourceBuffer.at(identifier);
		}
	}

	void DeleteResource(std::string identifier)
	{
		auto iterator = resourceBuffer.find(identifier);

		if (iterator != resourceBuffer.end())
		{
			currentSize -= iterator->second->GetSize();
			delete iterator->second;
			resourceBuffer.erase(identifier);
		}
	}

	void DeleteAllResources()
	{
		for (auto iter = resourceBuffer.begin(); iter != resourceBuffer.end(); iter++)
		{
			delete iter->second;
		}

		resourceBuffer.clear();
		currentSize = 0;
	}

	std::unordered_map<std::string, T*> & GetResourceBuffer()
	{
		return resourceBuffer;
	}

	size_t GetMaxSize() const
	{
		return maxSize;
	}

	size_t GetCurrentSize() const
	{
		return currentSize;
	}

private:
	std::string resourceManagerID;
	size_t maxSize;
	size_t currentSize;
	std::unordered_map<std::string, T*> resourceBuffer;
};