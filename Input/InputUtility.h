#pragma once

#include <map>

class InputUtility
{
public:
	static int GetKeyId(std::string keyName);

private:
	static const std::map<std::string, int> keyIDs;
};

