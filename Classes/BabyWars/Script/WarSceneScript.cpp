#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "WarSceneScript.h"
#include "TileMapScript.h"
#include "UnitMapScript.h"
#include "MovePathScript.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/Matrix2DDimension.h"
#include "../../BabyEngine/Utilities/GridIndex.h"
#include "../Event/EvtDataDragScene.h"
#include "../Event/EvtDataActivateUnitAtPosition.h"
#include "../Event/EvtDataDeactivateActiveUnit.h"
#include "../Event/EvtDataFinishMakeMovePath.h"
#include "../Event/EvtDataMakeMovePath.h"
#include "../Resource/ResourceLoader.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneScriptImpl and the touch state classes.
//////////////////////////////////////////////////////////////////////////
struct WarSceneScript::WarSceneScriptImpl
{
	WarSceneScriptImpl();
	~WarSceneScriptImpl();

	void initialize(std::weak_ptr<WarSceneScriptImpl> selfPtr);

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

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
	std::weak_ptr<MovePathScript> m_ChildMovePathScript;

	//Members for touch.
	class TouchManager;
	std::shared_ptr<TouchManager> m_TouchManager;
};

class WarSceneScript::WarSceneScriptImpl::TouchManager
{
public:
	TouchManager(){ m_TouchOneByOne->retain(); }
	~TouchManager(){ m_TouchOneByOne->release(); }

	void initialize(std::weak_ptr<TouchManager> selfPtr, std::weak_ptr<WarSceneScriptImpl> script);

	void enableTouch(bool enable);

	void saveCurrentStateToPrevious();
	void resumeCurrentStateFromPrevious();

	template <typename TouchState>
	void setCurrentState();

	std::shared_ptr<WarSceneScriptImpl> getScriptImpl() const;

	cocos2d::Vec2 m_TouchOneByOneBeganPosition;

private:
	class BaseTouchState;
	class TouchStateIdle;
	class TouchStateDraggingScene;
	class TouchStateActivatedUnit;
	class TouchStateMakingMovePath;

	std::shared_ptr<BaseTouchState> m_PreviousState;
	std::shared_ptr<BaseTouchState> m_CurrentState;

	std::unordered_map<std::type_index, std::shared_ptr<BaseTouchState>> m_States;

	std::weak_ptr<WarSceneScriptImpl> m_ScriptImpl;

	cocos2d::EventListenerTouchOneByOne * m_TouchOneByOne{ cocos2d::EventListenerTouchOneByOne::create() };
};

class WarSceneScript::WarSceneScriptImpl::TouchManager::BaseTouchState
{
public:
	virtual ~BaseTouchState() = default;

	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event) = 0;
	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) = 0;
	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) = 0;

protected:
	BaseTouchState(std::weak_ptr<TouchManager> manager) : m_Manager{ std::move(manager) }{}

	std::weak_ptr<TouchManager> m_Manager;
};

class WarSceneScript::WarSceneScriptImpl::TouchManager::TouchStateIdle : public BaseTouchState
{
public:
	TouchStateIdle(std::weak_ptr<TouchManager> manager) : BaseTouchState(manager){}
	virtual ~TouchStateIdle() = default;

protected:
	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		m_Manager.lock()->m_TouchOneByOneBeganPosition = touch->getLocation();
		return true;
	}

	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just starts dragging the scene, without doing any other things.
		//Just set the touch state to DraggingScene and set the position of the scene.
		auto manager = m_Manager.lock();
		manager->saveCurrentStateToPrevious();
		manager->setCurrentState<TouchStateDraggingScene>();

		manager->getScriptImpl()->setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just touch something without moving.
		//If he touches an unit, activate it and set the touch state to UnitActivated. Do nothing otherwise.
		auto manager = m_Manager.lock();
		auto scriptImpl = manager->getScriptImpl();
		if (scriptImpl->m_ChildUnitMapScript.lock()->activateUnitAtIndex(scriptImpl->toGridIndex(manager->m_TouchOneByOneBeganPosition)))
			manager->setCurrentState<TouchStateActivatedUnit>();
	}
};

class WarSceneScript::WarSceneScriptImpl::TouchManager::TouchStateDraggingScene : public BaseTouchState
{
public:
	TouchStateDraggingScene(std::weak_ptr<TouchManager> manager) : BaseTouchState(manager){}
	virtual ~TouchStateDraggingScene() = default;

protected:
	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		m_Manager.lock()->m_TouchOneByOneBeganPosition = touch->getLocation();
		return true;
	}

	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player continues dragging the scene, without doing any other things.
		//Just set the position of the scene.
		m_Manager.lock()->getScriptImpl()->setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just finishes dragging the scene.
		//Just set the touch state to the previous state and do nothing else.
		m_Manager.lock()->resumeCurrentStateFromPrevious();
	}
};

class WarSceneScript::WarSceneScriptImpl::TouchManager::TouchStateActivatedUnit : public BaseTouchState
{
public:
	TouchStateActivatedUnit(std::weak_ptr<TouchManager> manager) : BaseTouchState(manager){}
	virtual ~TouchStateActivatedUnit() = default;

protected:
	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		m_Manager.lock()->m_TouchOneByOneBeganPosition = touch->getLocation();
		return true;
	}

	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player drag something while an unit is activated. There are some possible meanings of the drag.

		//Some variables to make the job easier.
		auto manager = m_Manager.lock();
		auto scriptImpl = manager->getScriptImpl();
		auto unitMapScript = scriptImpl->m_ChildUnitMapScript.lock();
		auto gridIndex = scriptImpl->toGridIndex(manager->m_TouchOneByOneBeganPosition);

		//1. The player doesn't touch any units, or the unit he touches is not the activated unit. It means that he's dragging the scene.
		if (!unitMapScript->isUnitActiveAtIndex(gridIndex)){
			//Just set the touch state to DraggingScene and set the position of the scene.
			manager->saveCurrentStateToPrevious();
			manager->setCurrentState<TouchStateDraggingScene>();

			scriptImpl->setPositionWithOffsetAndBoundary(touch->getLocation() - touch->getPreviousLocation());
		}
		else{
			//2. The player is dragging the activated unit. It means that he's making a move path for the unit.
			//Set the touch state to DrawingMovePath and show the path as player touch.
			manager->setCurrentState<TouchStateMakingMovePath>();

			scriptImpl->makeMovePath(scriptImpl->toGridIndex(touch->getLocation()));
		}
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player touched something without moving. There are some possible meanings of the touch.
		//There will be much more conditions here. For now, we just assumes that the player is to activate another unit or deactivate the active unit.
		//#TODO: Add more conditions, such as the player touched a command and so on.
		auto manager = m_Manager.lock();
		auto script = manager->getScriptImpl();
		auto unitMapScript = script->m_ChildUnitMapScript.lock();
		auto gridIndex = script->toGridIndex(touch->getLocation());

		if (unitMapScript->isUnitActiveAtIndex(gridIndex)){
			//The player touched the active unit. It means that he'll deactivate it.
			unitMapScript->deactivateActiveUnit();
			manager->setCurrentState<TouchStateIdle>();
		}
		else{
			//The player touched an inactive unit or an empty grid.
			//Activate the unit (if it exist) and set the touch state corresponding to the activate result.
			if (!unitMapScript->activateUnitAtIndex(gridIndex))
				manager->setCurrentState<TouchStateIdle>();
		}
	}
};

class WarSceneScript::WarSceneScriptImpl::TouchManager::TouchStateMakingMovePath : public BaseTouchState
{
public:
	TouchStateMakingMovePath(std::weak_ptr<TouchManager> manager) : BaseTouchState(manager){}
	virtual ~TouchStateMakingMovePath() = default;

protected:
	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		m_Manager.lock()->m_TouchOneByOneBeganPosition = touch->getLocation();
		return true;
	}

	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player is continuing making move path for the active unit.
		//Show the path as he touch.
		auto scriptImpl = m_Manager.lock()->getScriptImpl();
		scriptImpl->makeMovePath(scriptImpl->toGridIndex(touch->getLocation()));
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player finishes making the move path.

		auto manager = m_Manager.lock();
		auto script = manager->getScriptImpl();
		auto touchBeganGridIndex = script->toGridIndex(manager->m_TouchOneByOneBeganPosition);

		if (script->isMovePathValid()){
			auto destination = script->toGridIndex(touch->getLocation());

			script->m_ChildUnitMapScript.lock()->deactivateAndMoveUnit(touchBeganGridIndex, destination);

			manager->setCurrentState<TouchStateIdle>();
		}
		else{
			script->clearMovePath();
			script->m_ChildUnitMapScript.lock()->deactivateActiveUnit();

			manager->setCurrentState<TouchStateIdle>();
		}
	}
};

//////////////////////////////////////////////////////////////////////////
//Implementation of SarSceneScriptImpl and TouchManager.
//////////////////////////////////////////////////////////////////////////
std::string WarSceneScript::WarSceneScriptImpl::s_TileMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_UnitMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_MovePathActorPath;

WarSceneScript::WarSceneScriptImpl::WarSceneScriptImpl() : m_TouchManager{ std::make_shared<TouchManager>() }
{
}

WarSceneScript::WarSceneScriptImpl::~WarSceneScriptImpl()
{
}

void WarSceneScript::WarSceneScriptImpl::initialize(std::weak_ptr<WarSceneScriptImpl> selfPtr)
{
	m_TouchManager->initialize(m_TouchManager, selfPtr);
}

void WarSceneScript::WarSceneScriptImpl::onActivateUnitAtPosition(const IEventData & e)
{
	const auto & activateUnitEvent = static_cast<const EvtDataActivateUnitAtPosition &>(e);
	m_ChildUnitMapScript.lock()->activateUnitAtIndex(toGridIndex(activateUnitEvent.getPosition()));
}

void WarSceneScript::WarSceneScriptImpl::onDeactivateActiveUnit(const IEventData & e)
{
	m_ChildUnitMapScript.lock()->deactivateActiveUnit();
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
	}
	else{
		m_ChildUnitMapScript.lock()->deactivateActiveUnit();
	}

	clearMovePath();
}

void WarSceneScript::WarSceneScriptImpl::onMakeMovePath(const IEventData & e)
{
	const auto & makePathEvent = static_cast<const EvtDataMakeMovePath &>(e);
	makeMovePath(toGridIndex(makePathEvent.getPosition()));
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

void WarSceneScript::WarSceneScriptImpl::TouchManager::initialize(std::weak_ptr<TouchManager> selfPtr, std::weak_ptr<WarSceneScriptImpl> script)
{
	m_ScriptImpl = std::move(script);

	m_States.emplace(typeid(TouchStateIdle), std::make_shared<TouchStateIdle>(selfPtr));
	m_States.emplace(typeid(TouchStateDraggingScene), std::make_shared<TouchStateDraggingScene>(selfPtr));
	m_States.emplace(typeid(TouchStateActivatedUnit), std::make_shared<TouchStateActivatedUnit>(selfPtr));
	m_States.emplace(typeid(TouchStateMakingMovePath), std::make_shared<TouchStateMakingMovePath>(selfPtr));

	setCurrentState<TouchStateIdle>();

	//Initialize the touch listener.
	m_TouchOneByOne->onTouchBegan = [this](cocos2d::Touch * touch, cocos2d::Event * event){
		return m_CurrentState->onTouchOneByOneBegan(touch, event);
	};
	m_TouchOneByOne->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event){
		m_CurrentState->onTouchOneByOneMoved(touch, event);
	};
	m_TouchOneByOne->onTouchEnded = [this](cocos2d::Touch * touch, cocos2d::Event * event){
		m_CurrentState->onTouchOneByOneEnded(touch, event);
	};
}

void WarSceneScript::WarSceneScriptImpl::TouchManager::enableTouch(bool enable)
{
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	if (!enable){
		eventDispatcher->removeEventListener(m_TouchOneByOne);
		return;
	}

	auto renderComponent = m_ScriptImpl.lock()->m_RenderComponent.lock();
	eventDispatcher->addEventListenerWithSceneGraphPriority(m_TouchOneByOne, renderComponent->getSceneNode());
}

void WarSceneScript::WarSceneScriptImpl::TouchManager::saveCurrentStateToPrevious()
{
	m_PreviousState = m_CurrentState;
}

void WarSceneScript::WarSceneScriptImpl::TouchManager::resumeCurrentStateFromPrevious()
{
	m_CurrentState = m_PreviousState;
}

template <typename TouchState>
void WarSceneScript::WarSceneScriptImpl::TouchManager::setCurrentState()
{
	auto stateIter = m_States.find(typeid(TouchState));
	assert(stateIter != m_States.end() && "WarSceneScriptImpl::TouchStateManager::setPreviousState() there's no state of the template parameter.");

	m_CurrentState = stateIter->second;
}

std::shared_ptr<WarSceneScript::WarSceneScriptImpl> WarSceneScript::WarSceneScriptImpl::TouchManager::getScriptImpl() const
{
	assert(!m_ScriptImpl.expired() && "WarSceneScriptImpl::TouchStateManager::getScript() the script is expired.");
	return m_ScriptImpl.lock();
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
	pimpl->initialize(pimpl);

	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarSceneScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");
	WarSceneScriptImpl::s_MovePathActorPath = relatedActorElement->Attribute("MovePath");

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

	//Create and add the move path.
	auto movePathActor = gameLogic->createActor(WarSceneScriptImpl::s_MovePathActorPath.c_str());
	pimpl->m_ChildMovePathScript = movePathActor->getComponent<MovePathScript>();
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

	//Listen to touch events for the scene.
	pimpl->m_TouchManager->enableTouch(true);
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
