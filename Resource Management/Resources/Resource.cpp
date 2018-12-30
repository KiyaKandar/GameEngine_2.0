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

std::string Resource::GetName()
{
	return name;
}

void Resource::SetName(std::string newName)
{
	name = newName;
}

size_t Resource::GetSize()
{
	return size;
}

void Resource::SetSize(size_t newSize)
{
	size = newSize;
}
