#include "XMLParser.h"

#include <fstream>
#include <iostream>

const int XML_DATA_START = 0;

std::vector<Node*> XMLParser::rootNodesToDelete = std::vector<Node*>();

XMLParser::XMLParser()
{
}

XMLParser::~XMLParser()
{
}

std::string XMLParser::LoadXmlFile(std::string filename)
{
	std::ifstream inFile(filename);

	if (!inFile) 
	{
		throw "Could not load File" + filename;
	}

	std::string xmlDump;
	std::string line;

	while (std::getline(inFile, line)) 
	{
		xmlDump += line;
	}

	std::vector<char> xmlData = std::vector<char>(xmlDump.begin(), xmlDump.end());
	xmlData.push_back('\0');

	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_no_data_nodes>(&xmlData[XML_DATA_START]);
	rapidxml::xml_node<>* firstNode = doc.first_node();

	parsedXml = new Node();
	RecursivelyParse(firstNode, &parsedXml);

	XMLParser::rootNodesToDelete.push_back(parsedXml);

	return parsedXml->nodeType;
}

void XMLParser::RecursivelyParse(rapidxml::xml_node<>* unParsedNode, Node** parsedNode)
{
	if (unParsedNode) 
	{
		(*parsedNode)->nodeType = unParsedNode->name();

		if (unParsedNode->first_attribute())
		{
			(*parsedNode)->name = unParsedNode->first_attribute()->value();
		}
		(*parsedNode)->value = unParsedNode->value();

		unsigned int i = 0;
		for(unParsedNode = unParsedNode->first_node(); unParsedNode != nullptr; unParsedNode = unParsedNode->next_sibling(), ++i)
		{
			(*parsedNode)->children.push_back(new Node());
			RecursivelyParse(unParsedNode, &(*parsedNode)->children[i]);
		}
	}
}

void XMLParser::DeleteAllParsedXml()
{
	for (Node* rootNode : XMLParser::rootNodesToDelete)
	{
		DeleteAllNodes(rootNode);
	}

	XMLParser::rootNodesToDelete.clear();
}

void XMLParser::DeleteAllNodes(Node* currentNode)
{
	for (Node* node : currentNode->children)
	{
		DeleteAllNodes(node);
	}
	delete currentNode;
	currentNode = nullptr;
}
