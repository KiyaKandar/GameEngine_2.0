#pragma once

#include <functional>
#include <vector>
#include "Database.h"

class TableCreation
{
public:
	explicit TableCreation(Database* database);
	~TableCreation();

	void AddTablesToDatabase() const;

private:
	void AddGameObjectsTable() const;
	void AddMeshesTable() const;
	void AddUiMeshTable() const;
	void AddSoundsTable() const;
	void AddLightsTable() const;

	std::vector<std::function<void()>> tableAdditions;
	Database* database;
};

