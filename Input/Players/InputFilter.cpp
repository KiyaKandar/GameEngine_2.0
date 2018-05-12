#include "InputFilter.h"

#include "../InputUtility.h"

InputFilter::InputFilter()
{

}

InputFilter::~InputFilter()
{

}

std::string InputFilter::loadConfigFile()
{
	return std::string();
}

std::vector<int> InputFilter::getListenedKeys(std::string &configInfo, std::string &separator)
{
	std::vector<int> listenedKeys;

	if (configInfo == "")
	{
		return listenedKeys;
	}

	std::string appendStr = configInfo + separator;

	size_t position = appendStr.find(separator);
	size_t size = appendStr.size();

	while (position != std::string::npos)
	{
		std::string token = appendStr.substr(0, position);
		listenedKeys.push_back(InputUtility::getKeyID(token));

		appendStr = appendStr.substr(position + 1, size);
		position = appendStr.find(separator);
	}

	return listenedKeys;
}
