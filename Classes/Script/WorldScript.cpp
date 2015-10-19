#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "WorldScript.h"
#include "TileScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
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

	void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);

	void loadTileMap(tinyxml2::XMLElement * xmlElement, Actor & worldActor);

	static std::string s_TileActorPath;

	std::string m_WorldDataPath;
	int m_RowCount{}, m_ColCount{};
	cocos2d::Size m_UntransformedWorldSize;
	std::vector<std::vector<std::shared_ptr<TileScript>>> m_TileMap;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string WorldScript::WorldScriptImpl::s_TileActorPath;

void WorldScript::WorldScriptImpl::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	assert(!m_RenderComponent.expired() && "WorldScriptImpl::onTouchMoved() the render component is expired.");
	auto strongRenderComponent = m_RenderComponent.lock();
	auto underlyingNode = strongRenderComponent->getSceneNode();

	//Calculate the new position.
	auto newPosition = touch->getLocation() - touch->getPreviousLocation() + underlyingNode->getPosition();

	//Modify the new position so that the player can't drag the map too far away.
	//Firstly, get the size of window, boundary and world.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto worldSize = m_UntransformedWorldSize;
	worldSize.width *= underlyingNode->getScaleX();
	worldSize.height *= underlyingNode->getScaleY();
	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(worldSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(worldSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;
	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX){
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = underlyingNode->getPosition().x;

	if (maxY > minY){
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = underlyingNode->getPosition().y;

	//Make use of the newPosition.
	underlyingNode->setPosition(newPosition);
}

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
		assert(rowIDs.size() == m_ColCount && "WorldScript::loadWorld() the columns count is less than the width of the world.");

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

	//Calculate the untransformed world size. Useful when the world is dragged.
	auto realGridSize = resourceLoader->getRealGameGridSize();
	m_UntransformedWorldSize.width = realGridSize.width * m_ColCount;
	m_UntransformedWorldSize.height = realGridSize.height * m_RowCount;

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto posX = -(m_UntransformedWorldSize.width - windowSize.width) / 2;
	auto posY = -(m_UntransformedWorldSize.height - windowSize.height) / 2;
	m_RenderComponent.lock()->getSceneNode()->setPosition(posX, posY);
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
	pimpl->m_RenderComponent = m_OwnerActor.lock()->getRenderComponent();

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

	//Load tile map and units and so on.
	pimpl->loadTileMap(rootElement, *m_OwnerActor.lock());

	//Enable dragging the world.
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [](cocos2d::Touch * touch, cocos2d::Event * event){return true; };
	touchListener->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event){
		pimpl->onTouchMoved(touch, event);
	};
	auto underlyingNode = pimpl->m_RenderComponent.lock()->getSceneNode();
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, underlyingNode);
}

const std::string WorldScript::Type = "WorldScript";

const std::string & WorldScript::getType() const
{
	return Type;
}
