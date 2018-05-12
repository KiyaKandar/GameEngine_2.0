#pragma once

#include <map>

class InputUtility
{
public:
	static int getKeyID(std::string keyName);

private:
	static const std::map<std::string, int> keyIDs;
};

