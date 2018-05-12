#include "Resource.h"

Resource::Resource()
{
	name = "";
	size = size_t(0);
}

Resource::Resource(std::string name, size_t size)
{
	this->name = name;
	this->size = size;
}

Resource::~Resource()
{
}

std::string Resource::getName()
{
	return name;
}

void Resource::setName(std::string newName)
{
	name = newName;
}

size_t Resource::getSize()
{
	return size;
}

void Resource::setSize(size_t newSize)
{
	size = newSize;
}
