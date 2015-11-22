#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/EvtDataInputDrag.h"
#include "../../BabyEngine/Event/EvtDataInputTouch.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Resource/ResourceLoader.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/Matrix2DDimension.h"
#include "../Utilities/MovingPath.h"
#include "MovingAreaScript.h"
#include "MovingPathScript.h"
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

	void onInputDrag(const IEventData & e);
	void onInputTouch(const IEventData & e);

	void activateUnitAndShowMovingArea(const cocos2d::Vec2 & position);
	void dragField(const cocos2d::Vec2 & offset);
	void deactivateActiveUnitAndClearMovingArea();
	void makeMovingPath(const cocos2d::Vec2 & position);
	void finishMakingMovingPath(const cocos2d::Vec2 & position);
	void setFieldPosition(const cocos2d::Vec2 & position);

	bool canActivateUnitAtPosition(const cocos2d::Vec2 & pos) const;
	bool isUnitActiveAtPosition(const cocos2d::Vec2 & pos) const;
	bool hasUnitActive() const;

	cocos2d::Size getMapSize() const;
	cocos2d::Vec2 toPositionInField(const cocos2d::Vec2 & position) const;
	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	GridIndex m_MovePathStartIndex{ -1, -1 };

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;
	static std::string s_MovingPathActorPath;
	static std::string s_MovingAreaActorPath;

	ActorID m_ActorID{ INVALID_ACTOR_ID };

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
	std::weak_ptr<MovingPathScript> m_ChildMovingPathScript;
	std::weak_ptr<MovingAreaScript> m_ChildMovingAreaScript;

	//////////////////////////////////////////////////////////////////////////
	//State stuffs.
	void initStateStuff(std::weak_ptr<WarSceneScriptImpl> scriptImpl);

private:
	class BaseState;
	class StateIdle;
	class StateDraggingField;
	class StateActivatedUnit;
	class StateMakingMovePath;

	template <typename State>
	void setCurrentState();

	void saveCurrentStateToPrevious();
	void resumeCurrentStateFromPrevious();

	std::shared_ptr<BaseState> m_PreviousState;
	std::shared_ptr<BaseState> m_CurrentState;
	std::unordered_map<std::type_index, std::shared_ptr<BaseState>> m_States;
};

std::string WarSceneScript::WarSceneScriptImpl::s_TileMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_UnitMapActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_MovingPathActorPath;
std::string WarSceneScript::WarSceneScriptImpl::s_MovingAreaActorPath;

//////////////////////////////////////////////////////////////////////////
//Definition of state stuff.
//////////////////////////////////////////////////////////////////////////
void WarSceneScript::WarSceneScriptImpl::initStateStuff(std::weak_ptr<WarSceneScriptImpl> scriptImpl)
{
	//Create the state instances.
	m_States.emplace(typeid(StateIdle), std::make_shared<StateIdle>(scriptImpl));
	m_States.emplace(typeid(StateDraggingField), std::make_shared<StateDraggingField>(scriptImpl));
	m_States.emplace(typeid(StateActivatedUnit), std::make_shared<StateActivatedUnit>(scriptImpl));
	m_States.emplace(typeid(StateMakingMovePath), std::make_shared<StateMakingMovePath>(scriptImpl));

	setCurrentState<StateIdle>();
}

template <typename State>
void WarSceneScript::WarSceneScriptImpl::setCurrentState()
{
	auto stateIter = m_States.find(typeid(State));
	assert(stateIter != m_States.end() && "WarSceneScriptImpl::setCurrentState() there's no state of the template parameter.");

	m_CurrentState = stateIter->second;
}

void WarSceneScript::WarSceneScriptImpl::saveCurrentStateToPrevious()
{
	m_PreviousState = m_CurrentState;
}

void WarSceneScript::WarSceneScriptImpl::resumeCurrentStateFromPrevious()
{
	m_CurrentState = m_PreviousState;
}

class WarSceneScript::WarSceneScriptImpl::BaseState
{
public:
	virtual ~BaseState() = default;

	virtual void onTouch(const EvtDataInputTouch & touch) = 0;
	virtual void onDrag(const EvtDataInputDrag & drag) = 0;

protected:
	BaseState(std::weak_ptr<WarSceneScriptImpl> scriptImpl) : m_ScriptImpl{ std::move(scriptImpl) } {}

	std::weak_ptr<WarSceneScriptImpl> m_ScriptImpl;
};

class WarSceneScript::WarSceneScriptImpl::StateIdle : public BaseState
{
public:
	StateIdle(std::weak_ptr<WarSceneScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateIdle() = default;

protected:
	virtual void onTouch(const EvtDataInputTouch & touch) override
	{
		//The player just touch something without moving.
		//If he touches an unit, activate it and set the touch state to UnitActivated. Do nothing otherwise.
		auto scriptImpl = m_ScriptImpl.lock();
		const auto & touchPosition = touch.getPositionInWorld();

		if (scriptImpl->canActivateUnitAtPosition(touchPosition)) {
			scriptImpl->activateUnitAndShowMovingArea(touchPosition);
			scriptImpl->setCurrentState<StateActivatedUnit>();
		}
	}

	virtual void onDrag(const EvtDataInputDrag & drag) override
	{
		const auto dragState = drag.getState();
		if (dragState == EvtDataInputDrag::DragState::Begin) {
			//The player just starts dragging the scene, without doing any other things.
			//Just set the touch state to DraggingScene and set the position of the scene.
			auto scriptImpl = m_ScriptImpl.lock();
			scriptImpl->saveCurrentStateToPrevious();
			scriptImpl->setCurrentState<StateDraggingField>();

			scriptImpl->dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
		}
		else if (dragState == EvtDataInputDrag::DragState::Dragging) {
			assert("WarSceneScriptImpl::StateIdle::onDrag() the drag state is logically invalid (Dragging).");
		}
		else {	//Must be DragState::End
			assert("WarSceneScriptImpl::StateIdle::onDrag() the drag state is logically invalid (End).");
		}
	}
};

class WarSceneScript::WarSceneScriptImpl::StateDraggingField : public BaseState
{
public:
	StateDraggingField(std::weak_ptr<WarSceneScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateDraggingField() = default;

protected:
	virtual void onTouch(const EvtDataInputTouch & touch) override
	{
		assert("WarSceneScriptImpl::StateDraggingField::onTouch() the touch should not happen when dragging scene.");
	}

	virtual void onDrag(const EvtDataInputDrag & drag) override
	{
		const auto dragState = drag.getState();
		if (dragState == EvtDataInputDrag::DragState::Begin) {
			assert("WarSceneScriptImpl::StateDraggingField::onDrag() the drag state is logically invalid (Begin).");
		}
		else if (dragState == EvtDataInputDrag::DragState::Dragging) {
			//The player continues dragging the scene, without doing any other things.
			//Just set the position of the scene.
			m_ScriptImpl.lock()->dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
		}
		else {	//Must be DragState::End
			//The player finishes dragging the scene.
			//Set the position of the scene and the touch state to the previous state.
			m_ScriptImpl.lock()->dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
			m_ScriptImpl.lock()->resumeCurrentStateFromPrevious();
		}
	}
};

class WarSceneScript::WarSceneScriptImpl::StateActivatedUnit : public BaseState
{
public:
	StateActivatedUnit(std::weak_ptr<WarSceneScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateActivatedUnit() = default;

protected:
	virtual void onTouch(const EvtDataInputTouch & touch) override
	{
		//The player touched something without moving. There are some possible meanings of the touch.
		//There will be much more conditions here. For now, we just assumes that the player is to activate another unit or deactivate the active unit.
		//#TODO: Add more conditions, such as the player touched a command and so on.
		auto scriptImpl = m_ScriptImpl.lock();
		auto touchPosition = touch.getPositionInWorld();

		if (scriptImpl->isUnitActiveAtPosition(touchPosition)) {
			//The player touched the active unit. It means that he'll deactivate it.
			scriptImpl->setCurrentState<StateIdle>();
			scriptImpl->deactivateActiveUnitAndClearMovingArea();
		}
		else {
			//The player touched an inactive unit or an empty grid.
			//Activate the unit (if can) and set the touch state corresponding to the activate result.
			scriptImpl->deactivateActiveUnitAndClearMovingArea();

			if (scriptImpl->canActivateUnitAtPosition(touchPosition))
				scriptImpl->activateUnitAndShowMovingArea(touchPosition);
			else
				scriptImpl->setCurrentState<StateIdle>();
		}
	}

	virtual void onDrag(const EvtDataInputDrag & drag) override
	{
		//The player drag something while an unit is activated. There are some possible meanings of the drag.
		//Some variables to make the job easier.
		auto scriptImpl = m_ScriptImpl.lock();
		const auto dragState = drag.getState();

		if (dragState == EvtDataInputDrag::DragState::Begin) {
			if (scriptImpl->isUnitActiveAtPosition(drag.getPreviousPositionInWorld())) {
				//1. The player is dragging the activated unit. It means that he's making a move path for the unit.
				//Set the touch state to DrawingMovePath and show the path as player touch.
				scriptImpl->setCurrentState<StateMakingMovePath>();

				scriptImpl->makeMovingPath(drag.getPositionInWorld());
			}
			else {
				//2. The player doesn't drag the activated unit. It means that he's dragging the scene.
				//Just set the touch state to DraggingScene and set the position of the scene.
				scriptImpl->saveCurrentStateToPrevious();
				scriptImpl->setCurrentState<StateDraggingField>();

				scriptImpl->dragField(drag.getPositionInWorld() - drag.getPreviousPositionInWorld());
			}
		}
		else if (dragState == EvtDataInputDrag::DragState::Dragging) {
			assert("WarSceneScriptImpl::StateActivatedUnit::onDrag() the drag state is logically invalid (Dragging).");
		}
		else {	//Must be DragState::End
			assert("WarSceneScriptImpl::StateActivatedUnit::onDrag() the drag state is logically invalid (End).");
		}
	}
};

class WarSceneScript::WarSceneScriptImpl::StateMakingMovePath : public BaseState
{
public:
	StateMakingMovePath(std::weak_ptr<WarSceneScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateMakingMovePath() = default;

protected:
	void onTouch(const EvtDataInputTouch & touch) override
	{
		assert("WarSceneScriptImpl::StateMakingMovingPath::onTouch() the touch should not happen when making moving path.");
	}

	void onDrag(const EvtDataInputDrag & drag) override
	{
		const auto dragState = drag.getState();
		if (dragState == EvtDataInputDrag::DragState::Begin) {
			assert("WarSceneScriptImpl::StateMakingMovingPath::onDrag() the drag state is logically invalid (Begin).");
		}
		else if (dragState == EvtDataInputDrag::DragState::Dragging) {
			//The player is continuing making move path for the active unit.
			//Show the path as he touch.
			m_ScriptImpl.lock()->makeMovingPath(drag.getPositionInWorld());
		}
		else {	//Must be DragState::End
			//The player finishes making the move path.
			auto scriptImpl = m_ScriptImpl.lock();
			scriptImpl->setCurrentState<StateIdle>();

			scriptImpl->finishMakingMovingPath(drag.getPositionInWorld());
		}
	}
};

//////////////////////////////////////////////////////////////////////////
//Other members of WarSceneScriptImpl.
//////////////////////////////////////////////////////////////////////////
void WarSceneScript::WarSceneScriptImpl::onInputDrag(const IEventData & e)
{
	const auto & eventDrag = static_cast<const EvtDataInputDrag &>(e);
	if (m_ActorID != eventDrag.getActorID())
		return;

	m_CurrentState->onDrag(eventDrag);
}

void WarSceneScript::WarSceneScriptImpl::onInputTouch(const IEventData & e)
{
	const auto & eventTouch = static_cast<const EvtDataInputTouch &>(e);
	if (m_ActorID != eventTouch.getActorID())
		return;

	m_CurrentState->onTouch(eventTouch);
}

void WarSceneScript::WarSceneScriptImpl::activateUnitAndShowMovingArea(const cocos2d::Vec2 & position)
{
	auto unitMapScript = m_ChildUnitMapScript.lock();
	unitMapScript->activateUnitAtIndex(toGridIndex(position));

	auto movingAreaScript = m_ChildMovingAreaScript.lock();
	movingAreaScript->clearAndShowArea(*unitMapScript->getActiveUnit(), *m_ChildTileMapScript.lock(), *unitMapScript);
}

void WarSceneScript::WarSceneScriptImpl::dragField(const cocos2d::Vec2 & offset)
{
	assert(!m_ChildTileMapScript.expired() && "WarSceneScriptImpl::setPositionWithinBoundary() the child tile map script is expired.");
	auto transformComponent = m_ChildTileMapScript.lock()->getTransformComponent();
	auto newPosition = transformComponent->getPosition() + offset;

	//////////////////////////////////////////////////////////////////////////
	//Modify the new position so that the scene can't be set too far away.
	//Firstly, get the size of window, boundary and scene.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto extraBoundarySize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	auto warSceneSize = getMapSize();
	warSceneSize.width *= transformComponent->getScaleX();
	warSceneSize.height *= transformComponent->getScaleY();

	//Secondly, use the sizes to calculate the possible min and max of the drag-to position.
	auto minX = -(warSceneSize.width - windowSize.width + extraBoundarySize.width);
	auto minY = -(warSceneSize.height - windowSize.height + extraBoundarySize.height);
	auto maxX = extraBoundarySize.width;
	auto maxY = extraBoundarySize.height;

	//Finally, modify the newPosition using the mins and maxs if needed.
	if (maxX > minX) {
		if (newPosition.x > maxX)	newPosition.x = maxX;
		if (newPosition.x < minX)	newPosition.x = minX;
	}
	else { //This means that the width of the map is less than the window. Just ignore horizontal part of the drag.
		newPosition.x = transformComponent->getPosition().x;
	}
	if (maxY > minY) {
		if (newPosition.y > maxY)	newPosition.y = maxY;
		if (newPosition.y < minY)	newPosition.y = minY;
	}
	else { //This means that the height of the map is less than the window. Just ignore vertical part of the drag.
		newPosition.y = transformComponent->getPosition().y;
	}

	//////////////////////////////////////////////////////////////////////////
	//Make use of the newPosition.
	setFieldPosition(newPosition);
}

void WarSceneScript::WarSceneScriptImpl::deactivateActiveUnitAndClearMovingArea()
{
	m_ChildUnitMapScript.lock()->deactivateActiveUnit();
	m_ChildMovingAreaScript.lock()->clearArea();
}

void WarSceneScript::WarSceneScriptImpl::makeMovingPath(const cocos2d::Vec2 & position)
{
	auto unitMapScript = m_ChildUnitMapScript.lock();
	auto activeUnit = unitMapScript->getActiveUnit();
	assert(activeUnit && "WarSceneScriptImpl::onMakeMovePath() there's no active unit.");

	auto destination = toGridIndex(position);
	m_ChildMovingPathScript.lock()->showPath(destination, m_ChildMovingAreaScript.lock()->getUnderlyingArea());

	//This is only for convenience and should be removed.
	if (m_MovePathStartIndex.rowIndex == -1 && m_MovePathStartIndex.colIndex == -1)
		m_MovePathStartIndex = destination;
}

void WarSceneScript::WarSceneScriptImpl::finishMakingMovingPath(const cocos2d::Vec2 & position)
{
	auto touchGridIndex = toGridIndex(position);

	auto movingPathScript = m_ChildMovingPathScript.lock();
	auto unitMapScript = m_ChildUnitMapScript.lock();
	auto movingUnit = unitMapScript->getActiveUnit();

	if (movingPathScript->isBackIndex(touchGridIndex) && unitMapScript->canUnitStayAtIndex(*movingUnit, touchGridIndex))
		m_ChildUnitMapScript.lock()->deactivateAndMoveUnit(*movingUnit, movingPathScript->getUnderlyingPath());
	else
		m_ChildUnitMapScript.lock()->deactivateActiveUnit();

	m_ChildMovingAreaScript.lock()->clearArea();
	movingPathScript->clearPath();

	//This is only for convenience and should be removed.
	m_MovePathStartIndex.rowIndex = -1;
	m_MovePathStartIndex.colIndex = -1;
}

void WarSceneScript::WarSceneScriptImpl::setFieldPosition(const cocos2d::Vec2 & position)
{
	m_ChildMovingAreaScript.lock()->setPosition(position);
	m_ChildMovingPathScript.lock()->setPosition(position);
	m_ChildTileMapScript.lock()->setPosition(position);
	m_ChildUnitMapScript.lock()->setPosition(position);
}

bool WarSceneScript::WarSceneScriptImpl::canActivateUnitAtPosition(const cocos2d::Vec2 & pos) const
{
	return m_ChildUnitMapScript.lock()->canActivateUnitAtIndex(toGridIndex(pos));
}

bool WarSceneScript::WarSceneScriptImpl::isUnitActiveAtPosition(const cocos2d::Vec2 & pos) const
{
	return m_ChildUnitMapScript.lock()->isUnitActiveAtIndex(toGridIndex(pos));
}

bool WarSceneScript::WarSceneScriptImpl::hasUnitActive() const
{
	return m_ChildUnitMapScript.lock()->getActiveUnit() != nullptr;
}

cocos2d::Size WarSceneScript::WarSceneScriptImpl::getMapSize() const
{
	auto tileMapDimension = m_ChildTileMapScript.lock()->getMapDimension();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();

	return{ gridSize.width * tileMapDimension.colCount, gridSize.height * tileMapDimension.rowCount };
}

cocos2d::Vec2 WarSceneScript::WarSceneScriptImpl::toPositionInField(const cocos2d::Vec2 & position) const
{
	return m_ChildTileMapScript.lock()->getTransformComponent()->convertToLocalSpace(position);
}

GridIndex WarSceneScript::WarSceneScriptImpl::toGridIndex(const cocos2d::Vec2 & positionInWindow) const
{
	auto positionInScene = toPositionInField(positionInWindow);
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	return GridIndex(positionInScene, gridSize);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneScript.
//////////////////////////////////////////////////////////////////////////
WarSceneScript::WarSceneScript() : pimpl{ std::make_shared<WarSceneScriptImpl>() }
{
	pimpl->initStateStuff(pimpl);
}

WarSceneScript::~WarSceneScript()
{
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
	//#TODO: After the transition of the scene, the position of the scene is reset to {0,0}. Do something to deal with it.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->getMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	pimpl->setFieldPosition({ posX, posY });
}

bool WarSceneScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarSceneScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");
	WarSceneScriptImpl::s_MovingPathActorPath = relatedActorElement->Attribute("MovePath");
	WarSceneScriptImpl::s_MovingAreaActorPath = relatedActorElement->Attribute("MovingRange");

	isStaticInitialized = true;
	return true;
}

void WarSceneScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	pimpl->m_ActorID = ownerActor->getID();

	//////////////////////////////////////////////////////////////////////////
	//Get components.
	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "WarSceneScript::vPostInit() the actor has no render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "WarSceneScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

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
	auto movingAreaActor = gameLogic->createActor(WarSceneScriptImpl::s_MovingAreaActorPath.c_str());
	pimpl->m_ChildMovingAreaScript = movingAreaActor->getComponent<MovingAreaScript>();
	ownerActor->addChild(*movingAreaActor);

	//Create and add the move path.
	auto movingPathActor = gameLogic->createActor(WarSceneScriptImpl::s_MovingPathActorPath.c_str());
	pimpl->m_ChildMovingPathScript = movingPathActor->getComponent<MovingPathScript>();
	ownerActor->addChild(*movingPathActor);

	//#TODO: create and add, commander, weather and so on...

	//////////////////////////////////////////////////////////////////////////
	//Attach to event dispatcher.
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataInputTouch::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onInputTouch(e);
	});
	eventDispatcher->vAddListener(EvtDataInputDrag::s_EventType, pimpl, [this](const IEventData & e) {
		pimpl->onInputDrag(e);
	});

	//////////////////////////////////////////////////////////////////////////
	//Load the test warScene.
	//#TODO: Only for testing and should be removed.
	loadWarScene("Data\\WarScene\\WarSceneData_TestWarScene.xml");
}

const std::string WarSceneScript::Type = "WarSceneScript";

const std::string & WarSceneScript::getType() const
{
	return Type;
}
