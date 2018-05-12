#pragma once

#include "../Extrernal Libs/rapidxml-1.13/rapidxml.hpp"

#include <string>
#include <vector>

struct Node
{
	std::string nodeType;
	std::string name;
	std::string value;
	std::vector<Node*> children;
};

class XMLParser
{
public:
	XMLParser();
	~XMLParser();

	std::string loadXMLFile(std::string filename);

	void recursivelyParse(rapidxml::xml_node<>* unParsedXml, Node** parsedNode);
	static void deleteAllParsedXML();
	Node* parsedXml;

private:
	static void deleteAllNodes(Node* currentNode);
	static std::vector<Node*> rootNodesToDelete;
};

