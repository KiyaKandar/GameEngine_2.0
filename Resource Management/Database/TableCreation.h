#pragma once

#include <functional>
#include <vector>
#include "Database.h"

class TableCreation
{
public:
	explicit TableCreation(Database* database);
	~TableCreation();

	void addTablesToDatabase() const;

private:
	void addGameObjectsTable() const;
	void addMeshesTable() const;
	void addUIMeshTable() const;
	void addSoundsTable() const;
	void addLightsTable() const;

	std::vector<std::function<void()>> tableAdditions;
	Database* database;
};

