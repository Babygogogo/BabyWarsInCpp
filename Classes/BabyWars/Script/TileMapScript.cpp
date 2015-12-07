#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/StringToVector.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/TileData.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2D.h"
#include "TileMapScript.h"
#include "TileScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TileMapScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TileMapScript::TileMapScriptImpl
{
	TileMapScriptImpl() = default;
	~TileMapScriptImpl() = default;

	static std::string s_TileActorPath;

	Matrix2D<std::weak_ptr<TileScript>> m_TileMap;

	std::weak_ptr<TransformComponent> m_TransformComponent;
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
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto ownerActor = m_OwnerActor.lock();
	auto selfSceneNode = ownerActor->getRenderComponent()->getSceneNode();
	const auto tilesElement = rootElement->FirstChildElement("Tile");
	const auto mapElement = rootElement->FirstChildElement("Map");

	//Start loading the tile indexes of the tile map.
	//Because the code regards the bottom row as the first row while the xml regards the up most row as the first row, we must read the rows in reverse order.
	auto rowElement = mapElement->FirstChildElement("Row");
	for (auto rowIndex = dimension.rowCount - 1; rowIndex < dimension.rowCount; --rowIndex) {
		assert(rowElement && "TileMapScript::loadTileMap() the rows count is less than the height of the TileMap.");

		//Load the tile indexes of the row.
		auto rowIndexes = utilities::toVector<std::string>(rowElement->Attribute("TileIndexes"));
		assert(rowIndexes.size() == dimension.colCount && "TileMapScript::loadTileMap() the columns count is less than the width of the TileMap.");

		//For each ID in the row, create an tile actor add the scripts into the tile map.
		for (auto colIndex = rowIndexes.size() * 0; colIndex < rowIndexes.size(); ++colIndex) {
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
			selfSceneNode->addChild(tileActor->getRenderComponent()->getSceneNode());
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

bool TileMapScript::canPassThrough(const std::string & movementType, const GridIndex & index) const
{
	if (!pimpl->m_TileMap.isIndexValid(index))
		return false;

	return pimpl->m_TileMap[index].lock()->getTileData()->getMovingCost(movementType) > 0;
}

int TileMapScript::getMovingCost(const std::string & movementType, const GridIndex & index) const
{
	if (!pimpl->m_TileMap.isIndexValid(index))
		return 0;

	return pimpl->m_TileMap[index].lock()->getTileData()->getMovingCost(movementType);
}

bool TileMapScript::vInit(const tinyxml2::XMLElement * xmlElement)
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
	auto ownerActor = m_OwnerActor.lock();

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "TileMapScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);
}

const std::string TileMapScript::Type = "TileMapScript";

const std::string & TileMapScript::getType() const
{
	return Type;
}
