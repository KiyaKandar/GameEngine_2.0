#pragma once
#include <vector>
#include <string>
#include <map>

class InputFilter
{
public:
	InputFilter();
	~InputFilter();

	std::string loadConfigFile();
	static std::vector<int> getListenedKeys(std::string &configInfo, std::string &separator);
};