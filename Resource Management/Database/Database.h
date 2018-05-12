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

	void addTable(const std::string tableName, Table<Resource>* table);
	Table<Resource>* getTable(const std::string tableName);
	std::vector<Table<Resource>*> getAllTables() const;

	void addResourceToTable(const std::string tableName, Resource* resource);
	void addResourceToTable(const std::string tableName, Node* node);

	const size_t CurrentSize() const;
	const size_t MaxSize() const;

private:
	std::map<std::string, Table<Resource>*> tables;
	std::vector<Table<Resource>*> tablesVector;
};

