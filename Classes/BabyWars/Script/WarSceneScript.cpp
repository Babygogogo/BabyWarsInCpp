#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../../BabyEngine/Utilities/Matrix2DDimension.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataActivateUnitAtPosition.h"
#include "../Event/EvtDataDeactivateActiveUnit.h"
#include "../Event/EvtDataDragScene.h"
#include "../Event/EvtDataFinishMakeMovePath.h"
#include "../Event/EvtDataMakeMovePath.h"
#include "../Resource/ResourceLoader.h"
#include "MovingPathScript.h"
#include "MovingAreaScript.h"
#include "TileMapScript.h"
#include "UnitMapScript.h"
#include "WarSceneScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneScriptImpl and the touch state classes.
//////////////////////////////////////////////////////////////////////////
struct WarSceneScript::WarSceneScriptImpl
{
	WarSceneScriptImpl() = default;
	~WarSceneScriptImpl() = default;

	void onActivateUnitAtPosition(const IEventData & e);
	void onDeactivateActiveUnit(const IEventData & e);
	void onDragScene(const IEventData & e);
	void onFinishMakeMovePath(const IEventData & e);
	void onMakeMovePath(const IEventData & e);

	cocos2d::Size getMapSize() const;
	cocos2d::Vec2 toPositionInScene(const cocos2d::Vec2 & position) const;
	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	void setPositionWithOffsetAndBoundary(const cocos2d::Vec2 & offset);

	void makeMovePath(const GridIndex & gridIndex);
	void clearMovePath();
	bool isMovePathValid() const;

	GridIndex m_MovePathStartIndex{ -1, -1 };

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;
	static std::string s_MovePathActorPath;
	static std::string s_MovingRangeActorPath;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
	std::weak_ptr<MovingPathScript> m_ChildMovingPathScript;
	std::weak_ptr<MovingAreaScript> m_ChildMovingRangeScript;
};

std::string WarSceneScript::WarSceneScriptImpl::s_TileMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_UnitMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_MovePathActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_MovingRangeActorPath;

void WarSceneScript::WarSceneScriptImpl::onActivateUnitAtPosition(const IEventData & e)
{
	const auto & activateUnitEvent = static_cast<const EvtDataActivateUnitAtPosition &>(e);
	auto gridIndex = toGridIndex(activateUnitEvent.getPosition());

	auto unitMapScript = m_ChildUnitMapScript.lock();
	unitMapScript->activateUnitAtIndex(gridIndex);

	auto movingRangeScript = m_ChildMovingRangeScript.lock();
	movingRangeScript->clearRange();
	movingRangeScript->showRange(gridIndex, *m_ChildTileMapScript.lock(), *unitMapScript);
}

void WarSceneScript::WarSceneScriptImpl::onDeactivateActiveUnit(const IEventData & e)
{
	m_ChildUnitMapScript.lock()->deactivateActiveUnit();
	m_ChildMovingRangeScript.lock()->clearRange();
}

void WarSceneScript::WarSceneScriptImpl::onDragScene(const IEventData & e)
{
	const auto & dragSceneEvent = static_cast<const EvtDataDragScene &>(e);
	setPositionWithOffsetAndBoundary(dragSceneEvent.getOffset());
}

void WarSceneScript::WarSceneScriptImpl::onFinishMakeMovePath(const IEventData & e)
{
	if (isMovePathValid()){
		const auto & finishEvent = static_cast<const EvtDataFinishMakeMovePath &>(e);
		auto destination = toGridIndex(finishEvent.getPosition());

		m_ChildUnitMapScript.lock()->deactivateAndMoveUnit(m_MovePathStartIndex, destination);
		m_ChildMovingRangeScript.lock()->clearRange();
	}
	else{
		m_ChildUnitMapScript.lock()->deactivateActiveUnit();
		m_ChildMovingRangeScript.lock()->clearRange();
	}

	clearMovePath();
}

void WarSceneScript::WarSceneScriptImpl::onMakeMovePath(const IEventData & e)
{
	auto unitMapScript = m_ChildUnitMapScript.lock();
	auto activeUnit = unitMapScript->getActiveUnit();
	assert(activeUnit && "WarSceneScriptImpl::onMakeMovePath() there's no active unit.");

	const auto & makePathEvent = static_cast<const EvtDataMakeMovePath &>(e);
	makeMovePath(toGridIndex(makePathEvent.getPosition()));
	//m_ChildMovingPathScript.lock()->showPath(toGridIndex(makePathEvent.getPosition()), activeUnit, *m_ChildTileMapScript.lock(), *unitMapScript);
}

cocos2d::Size WarSceneScript::WarSceneScriptImpl::getMapSize() const
{
	auto tileMapDimension = m_ChildTileMapScript.lock()->getMapDimension();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();

	return{ gridSize.width * tileMapDimension.colCount, gridSize.height * tileMapDimension.rowCount };
}

cocos2d::Vec2 WarSceneScript::WarSceneScriptImpl::toPositionInScene(const cocos2d::Vec2 & position) const
{
	return m_RenderComponent.lock()->getSceneNode()->convertToNodeSpace(position);
}

GridIndex WarSceneScript::WarSceneScriptImpl::toGridIndex(const cocos2d::Vec2 & positionInWindow) const
{
	auto positionInScene = toPositionInScene(positionInWindow);
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	return GridIndex(positionInScene, gridSize);
}

void WarSceneScript::WarSceneScriptImpl::setPositionWithOffsetAndBoundary(const cocos2d::Vec2 & offset)
{
	assert(!m_RenderComponent.expired() && "WarSceneScriptImpl::setPositionWithinBoundary() the render component is expired.");
	auto strongRenderComponent = m_RenderComponent.lock();
	auto underlyingNode = strongRenderComponent->getSceneNode();
	auto newPosition = underlyingNode->getPosition() + offset;

	//////////////////////////////////////////////////////////////////////////
	//Modify the new position so that the scene can't be set too far away.
	//Firstly, get the size of window, boundary and scene.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize();
	auto warSceneSize = getMapSize();
	warSceneSize.width *= underlyingNode->getScaleX();
	warSceneSize.height *= underlyingNode->getScaleY();

	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(warSceneSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(warSceneSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;

	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX){
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else{ //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = underlyingNode->getPosition().x;
	}
	if (maxY > minY){
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else{ //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = underlyingNode->getPosition().y;
	}

	//////////////////////////////////////////////////////////////////////////
	//Make use of the newPosition.
	underlyingNode->setPosition(newPosition);
}

void WarSceneScript::WarSceneScriptImpl::makeMovePath(const GridIndex & gridIndex)
{
	//#TODO: Complete this function.
	//auto destination = GridIndex(convertedPosition, SingletonContainer::getInstance()->get<ResourceLoader>()->getRealGameGridSize());
	//auto unitMap = m_ChildUnitMapScript.lock();
	//auto tileMap = m_ChildTileMapScript.lock();
	//auto movingUnit = unitMap->getActiveUnit();

	//m_ChildMovePathScript.lock()->updatePath(destination, movingUnit, *tileMap, *unitMap);

	//This is only for convenience and should be removed.
	if (m_MovePathStartIndex.rowIndex == -1 && m_MovePathStartIndex.colIndex == -1)
		m_MovePathStartIndex = gridIndex;
}

void WarSceneScript::WarSceneScriptImpl::clearMovePath()
{
	//#TODO: Complete this function.

	//This is only for convenience and should be removed.
	m_MovePathStartIndex.rowIndex = -1;
	m_MovePathStartIndex.colIndex = -1;
}

bool WarSceneScript::WarSceneScriptImpl::isMovePathValid() const
{
	//#TODO: Complete this function.
	return true;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneScript.
//////////////////////////////////////////////////////////////////////////
WarSceneScript::WarSceneScript() : pimpl{ std::make_shared<WarSceneScriptImpl>() }
{
}

WarSceneScript::~WarSceneScript()
{
}

bool WarSceneScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarSceneScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");
	WarSceneScriptImpl::s_MovePathActorPath = relatedActorElement->Attribute("MovePath");
	WarSceneScriptImpl::s_MovingRangeActorPath = relatedActorElement->Attribute("MovingRange");

	isStaticInitialized = true;
	return true;
}

void WarSceneScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	pimpl->m_RenderComponent = ownerActor->getRenderComponent();

	//////////////////////////////////////////////////////////////////////////
	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	//Create and add the tile map.
	auto tileMapActor = gameLogic->createActor(WarSceneScriptImpl::s_TileMapActorPath.c_str());
	pimpl->m_ChildTileMapScript = tileMapActor->getComponent<TileMapScript>();
	ownerActor->addChild(*tileMapActor);

	//Create and add the unit map.
	auto unitMapActor = gameLogic->createActor(WarSceneScriptImpl::s_UnitMapActorPath.c_str());
	pimpl->m_ChildUnitMapScript = unitMapActor->getComponent<UnitMapScript>();
	ownerActor->addChild(*unitMapActor);

	//Create and add the moving range.
	auto movingRangeActor = gameLogic->createActor(WarSceneScriptImpl::s_MovingRangeActorPath.c_str());
	pimpl->m_ChildMovingRangeScript = movingRangeActor->getComponent<MovingAreaScript>();
	ownerActor->addChild(*movingRangeActor);

	//Create and add the move path.
	auto movePathActor = gameLogic->createActor(WarSceneScriptImpl::s_MovePathActorPath.c_str());
	pimpl->m_ChildMovingPathScript = movePathActor->getComponent<MovingPathScript>();
	ownerActor->addChild(*movePathActor);

	//#TODO: create and add, commander, weather and so on...

	//////////////////////////////////////////////////////////////////////////
	//Attach to event dispatcher.
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataActivateUnitAtPosition::s_EventType, pimpl, [this](const IEventData & e){
		pimpl->onActivateUnitAtPosition(e);
	});
	eventDispatcher->vAddListener(EvtDataDeactivateActiveUnit::s_EventType, pimpl, [this](const IEventData & e){
		pimpl->onDeactivateActiveUnit(e);
	});
	eventDispatcher->vAddListener(EvtDataDragScene::s_EventType, pimpl, [this](const IEventData & e){
		pimpl->onDragScene(e);
	});
	eventDispatcher->vAddListener(EvtDataFinishMakeMovePath::s_EventType, pimpl, [this](const IEventData & e){
		pimpl->onFinishMakeMovePath(e);
	});
	eventDispatcher->vAddListener(EvtDataMakeMovePath::s_EventType, pimpl, [this](const IEventData & e){
		pimpl->onMakeMovePath(e);
	});

	//////////////////////////////////////////////////////////////////////////
	//Load the test warScene.
	//#TODO: Only for testing and should be removed.
	loadWarScene("Data\\WarScene\\WarSceneData_TestWarScene.xml");
}

void WarSceneScript::loadWarScene(const char * xmlPath)
{
	//Load the xml file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "WarSceneScript::loadWarScene() failed to load xml file.");

	//Load everything on the scene.
	auto dataPathElement = rootElement->FirstChildElement("DataPath");
	pimpl->m_ChildTileMapScript.lock()->loadTileMap(dataPathElement->FirstChildElement("TileMap")->Attribute("Value"));
	pimpl->m_ChildUnitMapScript.lock()->loadUnitMap(dataPathElement->FirstChildElement("UnitMap")->Attribute("Value"));
	//#TODO: Load weather, commander, ...

	//Check if the size of unit map and tile map is the same.
	auto tileMapScript = pimpl->m_ChildTileMapScript.lock();
	auto unitMapScript = pimpl->m_ChildUnitMapScript.lock();
	assert(unitMapScript->getMapDimension() == tileMapScript->getMapDimension() && "WarSceneScript::vPostInit() the size of the unit map is not the same as the tile map.");

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->getMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	pimpl->m_RenderComponent.lock()->getSceneNode()->setPosition(posX, posY);
}

bool WarSceneScript::canActivateUnitAtPosition(const cocos2d::Vec2 & pos) const
{
	auto gridIndex = pimpl->toGridIndex(pos);
	return pimpl->m_ChildUnitMapScript.lock()->canActivateUnitAtIndex(gridIndex);
}

bool WarSceneScript::isUnitActiveAtPosition(const cocos2d::Vec2 & pos) const
{
	auto gridIndex = pimpl->toGridIndex(pos);
	return pimpl->m_ChildUnitMapScript.lock()->isUnitActiveAtIndex(gridIndex);
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
