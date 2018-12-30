#pragma once
#include <string>

class Resource

{
public:
	Resource();
	Resource(std::string name, size_t size);
	virtual ~Resource();

	std::string GetName();
	void SetName(std::string newName);

	size_t GetSize();
	void SetSize(size_t newSize);

private:
	std::string name;
	size_t size;
};