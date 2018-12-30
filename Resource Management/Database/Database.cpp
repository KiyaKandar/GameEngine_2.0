#include "Database.h"

Database::Database()
{
}

Database::~Database()
{
}

void Database::AddTable(const std::string tableName, Table<Resource>* table)
{
	tables[tableName] = table;
	tablesVector.push_back(table);
}

Table<Resource>* Database::GetTable(const std::string tableName)
{
	return tables.at(tableName);
}

std::vector<Table<Resource>*> Database::GetAllTables() const
{
	return tablesVector;
}

void Database::AddResourceToTable(const std::string tableName, Resource* resource)
{
	tables.at(tableName)->AddNewResource(resource);
}

void Database::AddResourceToTable(const std::string tableName, Node* node)
{
	tables.at(tableName)->AddNewResource(node);
}

const size_t Database::CurrentSize() const
{
	size_t memory = 0;

	for (auto table : tables)
	{
		memory += table.second->GetAllResources()->GetCurrentSize();
	}

	return memory;
}

const size_t Database::MaxSize() const
{
	size_t memory = 0;

	for (auto table : tables)
	{
		memory += table.second->GetAllResources()->GetMaxSize();
	}

	return memory;
}