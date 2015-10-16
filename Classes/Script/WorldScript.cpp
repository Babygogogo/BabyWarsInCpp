#include <vector>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "WorldScript.h"
#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../GameLogic/GameLogic.h"
#include "../Resource/TileDataID.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/StringToVector.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WorldScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WorldScript::WorldScriptImpl
{
	WorldScriptImpl(){};
	~WorldScriptImpl(){};

	void loadTileMap(tinyxml2::XMLElement * xmlElement, Actor & worldActor);

	static std::string s_TileActorPath;

	std::string m_WorldDataPath;
	int m_RowCount{}, m_ColCount{};
	std::vector<std::vector<std::shared_ptr<TileScript>>> m_TileMap;
};

std::string WorldScript::WorldScriptImpl::s_TileActorPath;

void WorldScript::WorldScriptImpl::loadTileMap(tinyxml2::XMLElement * xmlElement, Actor & worldActor)
{
	//Load the tile map size.
	auto tileMapElement = xmlElement->FirstChildElement("TileMap");
	m_RowCount = tileMapElement->IntAttribute("Height");
	m_ColCount = tileMapElement->IntAttribute("Width");
	m_TileMap.resize(m_RowCount);

	//Some object to ease the following steps.
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();

	//Load the tile data ids of the tile map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = tileMapElement->FirstChildElement("Row");
	for (auto rowIndex = m_RowCount - 1; rowIndex >= 0; --rowIndex){
		assert(rowElement && "WorldScript::loadWorld() the rows count is less than the height of the world.");

		//Load the TileDataIDs of the row.
		auto rowIDs = utilities::toVector<TileDataID>(rowElement->Attribute("TileDataIDs"));
		assert(rowIDs.size() == m_ColCount && "WorldScript::loadWorld() the rows count is less than the width of the world.");

		//For each ID in the row, create an tile actor add the scripts into the tile map.
		for (auto colIndex = 0; colIndex < rowIDs.size(); ++colIndex){
			//Create a new tile actor and initialize it with the id and indexes.
			auto tileActor = gameLogic->createActor(WorldScriptImpl::s_TileActorPath.c_str());
			auto tileScript = tileActor->getComponent<TileScript>();
			tileScript->setTileData(resourceLoader->getTileData(rowIDs[colIndex]));
			tileScript->setRowAndColIndex(rowIndex, colIndex);

			//Add the tile actor and script to world.
			worldActor.addChild(*tileActor);
			m_TileMap[rowIndex].emplace_back(std::move(tileScript));
		}

		//Load the next row of the tile map.
		rowElement = rowElement->NextSiblingElement();
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
WorldScript::WorldScript() : pimpl{ std::make_unique<WorldScriptImpl>() }
{
}

WorldScript::~WorldScript()
{
}

bool WorldScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Get the world file path.
	//#TODO: Only for testing and should be removed.
	pimpl->m_WorldDataPath = xmlElement->Attribute("TestWorld");

	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WorldScriptImpl::s_TileActorPath = relatedActorElement->Attribute("Tile");

	isStaticInitialized = true;
	return true;
}

void WorldScript::vPostInit()
{
	//Load the test world.
	//#TODO: Only for testing and should be removed.
	if (!pimpl->m_WorldDataPath.empty())
		loadWorld(pimpl->m_WorldDataPath.c_str());
}

void WorldScript::loadWorld(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "WorldScript::loadTileMap() failed to load xml file.");

	pimpl->loadTileMap(rootElement, *m_Actor.lock());
}

const std::string & WorldScript::getType() const
{
	return Type;
}

const std::string WorldScript::Type = "WorldScript";
