#include <vector>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "WorldScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WorldScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WorldScript::WorldScriptImpl
{
	WorldScriptImpl(){};
	~WorldScriptImpl(){};

	int m_RowCount{}, m_ColCount{};
	//	std::vector<std::vector<
};

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
WorldScript::WorldScript() : pimpl{ std::make_unique<WorldScriptImpl>() }
{
}

WorldScript::~WorldScript()
{
}

void WorldScript::loadWorld(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "WorldScript::loadTileMap() failed to load xml file.");

	//Load the tile map.
	auto tileMapElement = rootElement->FirstChildElement("TileMap");
	pimpl->m_RowCount = tileMapElement->IntAttribute("Height");
	pimpl->m_ColCount = tileMapElement->IntAttribute("Width");

	for (auto rowIndex = 0; rowIndex < pimpl->m_RowCount; ++rowIndex){
	}
}

bool WorldScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void WorldScript::vPostInit()
{
}

const std::string & WorldScript::getType() const
{
	return Type;
}

const std::string WorldScript::Type = "WorldScript";
