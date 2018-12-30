#pragma once

#include "../Resources/Resource.h"
#include "Table.h"

#include <string>
#include <map>

class Database
{
public:
	Database();
	~Database();

	void AddTable(const std::string tableName, Table<Resource>* table);
	Table<Resource>* GetTable(const std::string tableName);
	std::vector<Table<Resource>*> GetAllTables() const;

	void AddResourceToTable(const std::string tableName, Resource* resource);
	void AddResourceToTable(const std::string tableName, Node* node);

	const size_t CurrentSize() const;
	const size_t MaxSize() const;

private:
	std::map<std::string, Table<Resource>*> tables;
	std::vector<Table<Resource>*> tablesVector;
};

