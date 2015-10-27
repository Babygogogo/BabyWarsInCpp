#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TileMapScript.h"
#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../GameLogic/GameLogic.h"
#include "../Resource/TileDataID.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/StringToVector.h"
#include "../Utilities/SingletonContainer.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2D.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileMapScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileMapScript::TileMapScriptImpl
{
	TileMapScriptImpl(){};
	~TileMapScriptImpl(){};

	static std::string s_TileActorPath;

	Matrix2D<std::weak_ptr<TileScript>> m_TileMap;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string TileMapScript::TileMapScriptImpl::s_TileActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of TileMapScript.
//////////////////////////////////////////////////////////////////////////
TileMapScript::TileMapScript() : pimpl{ std::make_unique<TileMapScriptImpl>() }
{
}

TileMapScript::~TileMapScript()
{
}

bool TileMapScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	TileMapScriptImpl::s_TileActorPath = relatedActorElement->Attribute("Tile");

	isStaticInitialized = true;
	return true;
}

void TileMapScript::vPostInit()
{
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();
}

void TileMapScript::loadTileMap(const char * xmlPath)
{
	//////////////////////////////////////////////////////////////////////////
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "TileMapScript::loadTileMap() failed to load xml file.");

	//Load the tile map size.
	const auto dimension = Matrix2DDimension(rootElement->IntAttribute("Height"), rootElement->IntAttribute("Width"));
	pimpl->m_TileMap.setDimension(dimension);

	//////////////////////////////////////////////////////////////////////////
	//Load the map.
	//Some variables to make the job easier.
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	auto ownerActor = m_OwnerActor.lock();
	const auto tilesElement = rootElement->FirstChildElement("Tile");
	const auto mapElement = rootElement->FirstChildElement("Map");

	//Start loading the tile indexes of the tile map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = mapElement->FirstChildElement("Row");
	for (auto rowIndex = dimension.rowCount - 1; rowIndex < dimension.rowCount; --rowIndex){
		assert(rowElement && "TileMapScript::loadTileMap() the rows count is less than the height of the TileMap.");

		//Load the tile indexes of the row.
		auto rowIndexes = utilities::toVector<std::string>(rowElement->Attribute("TileIndexes"));
		assert(rowIndexes.size() == dimension.colCount && "TileMapScript::loadTileMap() the columns count is less than the width of the TileMap.");

		//For each ID in the row, create an tile actor add the scripts into the tile map.
		for (auto colIndex = rowIndexes.size() * 0; colIndex < rowIndexes.size(); ++colIndex){
			auto tileElement = tilesElement->FirstChildElement((std::string("Index") + rowIndexes[colIndex]).c_str());
			if (!tileElement)
				continue;

			//Create a new tile actor and initialize it with the id and indexes.
			auto tileActor = gameLogic->createActor(TileMapScriptImpl::s_TileActorPath.c_str());
			auto tileScript = tileActor->getComponent<TileScript>();
			tileScript->LoadTile(tileElement);
			const auto gridIndex = GridIndex(rowIndex, colIndex);
			tileScript->setGridIndexAndPosition(gridIndex);

			//Add the tile actor and script to TileMap.
			ownerActor->addChild(*tileActor);
			pimpl->m_TileMap[gridIndex] = tileScript;
		}

		//Load the next row of the tile map.
		rowElement = rowElement->NextSiblingElement();
	}
}

Matrix2DDimension TileMapScript::getMapDimension() const
{
	return pimpl->m_TileMap.getDimension();
}

const std::string TileMapScript::Type = "TileMapScript";

const std::string & TileMapScript::getType() const
{
	return Type;
}
