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
#include "WarFieldScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarFieldScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WarFieldScript::WarFieldScriptImpl
{
	WarFieldScriptImpl() = default;
	~WarFieldScriptImpl() = default;

	void activateUnitAndShowMovingArea(const cocos2d::Vec2 & position);
	void dragField(const cocos2d::Vec2 & offset);
	void deactivateActiveUnitAndClearMovingArea();
	void makeMovingPath(const cocos2d::Vec2 & position);
	void finishMakingMovingPath(const cocos2d::Vec2 & position);

	bool canActivateUnitAtPosition(const cocos2d::Vec2 & pos) const;
	bool isUnitActiveAtPosition(const cocos2d::Vec2 & pos) const;

	cocos2d::Size getMapSize() const;
	cocos2d::Vec2 toPositionInField(const cocos2d::Vec2 & positionInWindow) const;
	GridIndex toGridIndex(const cocos2d::Vec2 & positionInWindow) const;

	GridIndex m_MovePathStartIndex{ -1, -1 };

	static std::string s_TileMapActorPath;
	static std::string s_UnitMapActorPath;
	static std::string s_MovingPathActorPath;
	static std::string s_MovingAreaActorPath;

	std::weak_ptr<TransformComponent> m_TransformComponent;

	std::weak_ptr<TileMapScript> m_ChildTileMapScript;
	std::weak_ptr<UnitMapScript> m_ChildUnitMapScript;
	std::weak_ptr<MovingPathScript> m_ChildMovingPathScript;
	std::weak_ptr<MovingAreaScript> m_ChildMovingAreaScript;

	//////////////////////////////////////////////////////////////////////////
	//State stuffs.
	class BaseState;
	class StateIdle;
	class StateDraggingField;
	class StateActivatedUnit;
	class StateMakingMovingPath;

	void initStateStuff(std::weak_ptr<WarFieldScriptImpl> scriptImpl);

	std::shared_ptr<BaseState> m_PreviousState;
	std::shared_ptr<BaseState> m_CurrentState;

private:
	template <typename State>
	void setCurrentState();

	void saveCurrentStateToPrevious();
	void resumeCurrentStateFromPrevious();

	std::unordered_map<std::type_index, std::shared_ptr<BaseState>> m_States;
};

std::string WarFieldScript::WarFieldScriptImpl::s_TileMapActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_UnitMapActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_MovingPathActorPath;
std::string WarFieldScript::WarFieldScriptImpl::s_MovingAreaActorPath;

//////////////////////////////////////////////////////////////////////////
//Definition of state stuff.
//////////////////////////////////////////////////////////////////////////
void WarFieldScript::WarFieldScriptImpl::initStateStuff(std::weak_ptr<WarFieldScriptImpl> scriptImpl)
{
	//Create the state instances.
	m_States.emplace(typeid(StateIdle), std::make_shared<StateIdle>(scriptImpl));
	m_States.emplace(typeid(StateDraggingField), std::make_shared<StateDraggingField>(scriptImpl));
	m_States.emplace(typeid(StateActivatedUnit), std::make_shared<StateActivatedUnit>(scriptImpl));
	m_States.emplace(typeid(StateMakingMovingPath), std::make_shared<StateMakingMovingPath>(scriptImpl));

	setCurrentState<StateIdle>();
}

template <typename State>
void WarFieldScript::WarFieldScriptImpl::setCurrentState()
{
	auto stateIter = m_States.find(typeid(State));
	assert(stateIter != m_States.end() && "WarFieldScriptImpl::setCurrentState() there's no state of the template parameter.");

	m_CurrentState = stateIter->second;
}

void WarFieldScript::WarFieldScriptImpl::saveCurrentStateToPrevious()
{
	m_PreviousState = m_CurrentState;
}

void WarFieldScript::WarFieldScriptImpl::resumeCurrentStateFromPrevious()
{
	m_CurrentState = m_PreviousState;
}

class WarFieldScript::WarFieldScriptImpl::BaseState
{
public:
	virtual ~BaseState() = default;

	//Deal with the input event. The return value indicates whether the function swallows the event or not.
	virtual bool onTouch(const EvtDataInputTouch & touch) = 0;
	virtual bool onDrag(const EvtDataInputDrag & drag) = 0;

protected:
	BaseState(std::weak_ptr<WarFieldScriptImpl> scriptImpl) : m_ScriptImpl{ std::move(scriptImpl) } {}

	std::weak_ptr<WarFieldScriptImpl> m_ScriptImpl;
};

class WarFieldScript::WarFieldScriptImpl::StateIdle : public BaseState
{
public:
	StateIdle(std::weak_ptr<WarFieldScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateIdle() = default;

protected:
	virtual bool onTouch(const EvtDataInputTouch & touch) override
	{
		//The player just touch something without moving.
		//If he touches an unit, activate it and set the touch state to UnitActivated. Do nothing otherwise.
		auto scriptImpl = m_ScriptImpl.lock();
		const auto & touchPosition = touch.getPositionInWorld();

		if (scriptImpl->canActivateUnitAtPosition(touchPosition)) {
			scriptImpl->activateUnitAndShowMovingArea(touchPosition);
			scriptImpl->setCurrentState<StateActivatedUnit>();
		}

		return true;
	}

	virtual bool onDrag(const EvtDataInputDrag & drag) override
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
			assert("WarFieldScriptImpl::StateIdle::onDrag() the drag state is logically invalid (Dragging).");
		}
		else {	//Must be DragState::End
			assert("WarFieldScriptImpl::StateIdle::onDrag() the drag state is logically invalid (End).");
		}

		return true;
	}
};

class WarFieldScript::WarFieldScriptImpl::StateDraggingField : public BaseState
{
public:
	StateDraggingField(std::weak_ptr<WarFieldScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateDraggingField() = default;

protected:
	virtual bool onTouch(const EvtDataInputTouch & touch) override
	{
		assert("WarFieldScriptImpl::StateDraggingField::onTouch() the touch should not happen when dragging scene.");
		return true;
	}

	virtual bool onDrag(const EvtDataInputDrag & drag) override
	{
		const auto dragState = drag.getState();
		if (dragState == EvtDataInputDrag::DragState::Begin) {
			assert("WarFieldScriptImpl::StateDraggingField::onDrag() the drag state is logically invalid (Begin).");
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

		return true;
	}
};

class WarFieldScript::WarFieldScriptImpl::StateActivatedUnit : public BaseState
{
public:
	StateActivatedUnit(std::weak_ptr<WarFieldScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateActivatedUnit() = default;

protected:
	virtual bool onTouch(const EvtDataInputTouch & touch) override
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

		return true;
	}

	virtual bool onDrag(const EvtDataInputDrag & drag) override
	{
		//The player drag something while an unit is activated. There are some possible meanings of the drag.
		//Some variables to make the job easier.
		auto scriptImpl = m_ScriptImpl.lock();
		const auto dragState = drag.getState();

		if (dragState == EvtDataInputDrag::DragState::Begin) {
			if (scriptImpl->isUnitActiveAtPosition(drag.getPreviousPositionInWorld())) {
				//1. The player is dragging the activated unit. It means that he's making a move path for the unit.
				//Set the touch state to DrawingMovePath and show the path as player touch.
				scriptImpl->setCurrentState<StateMakingMovingPath>();

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
			assert("WarFieldScriptImpl::StateActivatedUnit::onDrag() the drag state is logically invalid (Dragging).");
		}
		else {	//Must be DragState::End
			assert("WarFieldScriptImpl::StateActivatedUnit::onDrag() the drag state is logically invalid (End).");
		}

		return true;
	}
};

class WarFieldScript::WarFieldScriptImpl::StateMakingMovingPath : public BaseState
{
public:
	StateMakingMovingPath(std::weak_ptr<WarFieldScriptImpl> scriptImpl) : BaseState(std::move(scriptImpl)) {}
	virtual ~StateMakingMovingPath() = default;

protected:
	bool onTouch(const EvtDataInputTouch & touch) override
	{
		assert("WarFieldScriptImpl::StateMakingMovingPath::onTouch() the touch should not happen when making moving path.");
		return true;
	}

	bool onDrag(const EvtDataInputDrag & drag) override
	{
		const auto dragState = drag.getState();
		if (dragState == EvtDataInputDrag::DragState::Begin) {
			assert("WarFieldScriptImpl::StateMakingMovingPath::onDrag() the drag state is logically invalid (Begin).");
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
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
//Other members of WarFieldScriptImpl.
//////////////////////////////////////////////////////////////////////////
void WarFieldScript::WarFieldScriptImpl::activateUnitAndShowMovingArea(const cocos2d::Vec2 & position)
{
	auto unitMapScript = m_ChildUnitMapScript.lock();
	unitMapScript->activateUnitAtIndex(toGridIndex(position));

	auto movingAreaScript = m_ChildMovingAreaScript.lock();
	movingAreaScript->clearAndShowArea(*unitMapScript->getActiveUnit(), *m_ChildTileMapScript.lock(), *unitMapScript);
}

void WarFieldScript::WarFieldScriptImpl::dragField(const cocos2d::Vec2 & offset)
{
	assert(!m_ChildTileMapScript.expired() && "WarFieldScriptImpl::setPositionWithinBoundary() the child tile map script is expired.");
	auto transformComponent = m_TransformComponent.lock();
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
	transformComponent->setPosition(newPosition);
}

void WarFieldScript::WarFieldScriptImpl::deactivateActiveUnitAndClearMovingArea()
{
	m_ChildUnitMapScript.lock()->deactivateActiveUnit();
	m_ChildMovingAreaScript.lock()->clearArea();
}

void WarFieldScript::WarFieldScriptImpl::makeMovingPath(const cocos2d::Vec2 & position)
{
	auto unitMapScript = m_ChildUnitMapScript.lock();
	auto activeUnit = unitMapScript->getActiveUnit();
	assert(activeUnit && "WarFieldScriptImpl::onMakeMovePath() there's no active unit.");

	auto destination = toGridIndex(position);
	m_ChildMovingPathScript.lock()->showPath(destination, m_ChildMovingAreaScript.lock()->getUnderlyingArea());

	//This is only for convenience and should be removed.
	if (m_MovePathStartIndex.rowIndex == -1 && m_MovePathStartIndex.colIndex == -1)
		m_MovePathStartIndex = destination;
}

void WarFieldScript::WarFieldScriptImpl::finishMakingMovingPath(const cocos2d::Vec2 & position)
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

bool WarFieldScript::WarFieldScriptImpl::canActivateUnitAtPosition(const cocos2d::Vec2 & pos) const
{
	return m_ChildUnitMapScript.lock()->canActivateUnitAtIndex(toGridIndex(pos));
}

bool WarFieldScript::WarFieldScriptImpl::isUnitActiveAtPosition(const cocos2d::Vec2 & pos) const
{
	return m_ChildUnitMapScript.lock()->isUnitActiveAtIndex(toGridIndex(pos));
}

cocos2d::Size WarFieldScript::WarFieldScriptImpl::getMapSize() const
{
	auto tileMapDimension = m_ChildTileMapScript.lock()->getMapDimension();
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();

	return{ gridSize.width * tileMapDimension.colCount, gridSize.height * tileMapDimension.rowCount };
}

cocos2d::Vec2 WarFieldScript::WarFieldScriptImpl::toPositionInField(const cocos2d::Vec2 & positionInWindow) const
{
	return m_TransformComponent.lock()->convertToLocalSpace(positionInWindow);
}

GridIndex WarFieldScript::WarFieldScriptImpl::toGridIndex(const cocos2d::Vec2 & positionInWindow) const
{
	auto positionInScene = toPositionInField(positionInWindow);
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	return GridIndex(positionInScene, gridSize);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarFieldScript.
//////////////////////////////////////////////////////////////////////////
WarFieldScript::WarFieldScript() : pimpl{ std::make_shared<WarFieldScriptImpl>() }
{
	pimpl->initStateStuff(pimpl);
}

WarFieldScript::~WarFieldScript()
{
}

bool WarFieldScript::onInputTouch(const EvtDataInputTouch & touch)
{
	return pimpl->m_CurrentState->onTouch(touch);
}

bool WarFieldScript::onInputDrag(const EvtDataInputDrag & drag)
{
	return pimpl->m_CurrentState->onDrag(drag);
}

void WarFieldScript::loadWarField(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "WarFieldScript::loadWarField() the xml element is nullptr.");

	//Load everything on the scene.
	pimpl->m_ChildTileMapScript.lock()->loadTileMap(xmlElement->FirstChildElement("TileMap")->Attribute("Value"));
	pimpl->m_ChildUnitMapScript.lock()->loadUnitMap(xmlElement->FirstChildElement("UnitMap")->Attribute("Value"));
	//#TODO: Load weather, commander, ...

	//Check if the size of unit map and tile map is the same.
	auto tileMapScript = pimpl->m_ChildTileMapScript.lock();
	auto unitMapScript = pimpl->m_ChildUnitMapScript.lock();
	assert(unitMapScript->getMapDimension() == tileMapScript->getMapDimension() && "WarFieldScript::loadWarField() the size of the unit map is not the same as the tile map.");

	//Set the position of the map so that the map is displayed in the middle of the window.
	auto windowSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	auto sceneSize = pimpl->getMapSize();
	auto posX = -(sceneSize.width - windowSize.width) / 2;
	auto posY = -(sceneSize.height - windowSize.height) / 2;
	pimpl->m_TransformComponent.lock()->setPosition({ posX, posY });
}

bool WarFieldScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarFieldScriptImpl::s_TileMapActorPath = relatedActorElement->Attribute("TileMap");
	WarFieldScriptImpl::s_UnitMapActorPath = relatedActorElement->Attribute("UnitMap");
	WarFieldScriptImpl::s_MovingPathActorPath = relatedActorElement->Attribute("MovePath");
	WarFieldScriptImpl::s_MovingAreaActorPath = relatedActorElement->Attribute("MovingRange");

	isStaticInitialized = true;
	return true;
}

void WarFieldScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();

	//////////////////////////////////////////////////////////////////////////
	//Get components.
	//auto renderComponent = ownerActor->getRenderComponent();
	//assert(renderComponent && "WarSceneScript::vPostInit() the actor has no render component.");
	//pimpl->m_RenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "WarSceneScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	//////////////////////////////////////////////////////////////////////////
	//Create and add child actors.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	//Create and add the tile map.
	auto tileMapActor = gameLogic->createActor(WarFieldScriptImpl::s_TileMapActorPath.c_str());
	pimpl->m_ChildTileMapScript = tileMapActor->getComponent<TileMapScript>();
	ownerActor->addChild(*tileMapActor);

	//Create and add the unit map.
	auto unitMapActor = gameLogic->createActor(WarFieldScriptImpl::s_UnitMapActorPath.c_str());
	pimpl->m_ChildUnitMapScript = unitMapActor->getComponent<UnitMapScript>();
	ownerActor->addChild(*unitMapActor);

	//Create and add the moving range.
	auto movingAreaActor = gameLogic->createActor(WarFieldScriptImpl::s_MovingAreaActorPath.c_str());
	pimpl->m_ChildMovingAreaScript = movingAreaActor->getComponent<MovingAreaScript>();
	ownerActor->addChild(*movingAreaActor);

	//Create and add the move path.
	auto movingPathActor = gameLogic->createActor(WarFieldScriptImpl::s_MovingPathActorPath.c_str());
	pimpl->m_ChildMovingPathScript = movingPathActor->getComponent<MovingPathScript>();
	ownerActor->addChild(*movingPathActor);

	//#TODO: create and add weather and so on...
}

const std::string WarFieldScript::Type{ "WarFieldScript" };

const std::string & WarFieldScript::getType() const
{
	return Type;
}
