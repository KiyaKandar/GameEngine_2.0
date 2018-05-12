#pragma once
#include <string>

class Resource

{
public:
	Resource();
	Resource(std::string name, size_t size);
	virtual ~Resource();

	std::string getName();
	void setName(std::string newName);

	size_t getSize();
	void setSize(size_t newSize);

private:
	std::string name;
	size_t size;
};