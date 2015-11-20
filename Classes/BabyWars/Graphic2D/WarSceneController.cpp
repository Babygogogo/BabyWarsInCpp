#include <cassert>

#include "cocos2d.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataActivateUnitAtPosition.h"
#include "../Event/EvtDataDeactivateActiveUnit.h"
#include "../Event/EvtDataDragScene.h"
#include "../Event/EvtDataFinishMakeMovingPath.h"
#include "../Event/EvtDataMakeMovingPath.h"
#include "../Script/WarSceneScript.h"
#include "WarSceneController.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneControllerImpl.
//////////////////////////////////////////////////////////////////////////
struct WarSceneController::WarSceneControllerImpl
{
	WarSceneControllerImpl();
	~WarSceneControllerImpl();

	void init(std::weak_ptr<WarSceneControllerImpl> self);

	void queueEventDragScene(const cocos2d::Vec2 & offset) const;
	void queueEventActivateUnitAtPosition(const cocos2d::Vec2 & pos) const;
	void queueEventDeactivateActiveUnit() const;
	void queueEventMakeMovePath(const cocos2d::Vec2 & pos) const;
	void queueEventFinishMakeMovePath(const cocos2d::Vec2 & pos) const;

	void saveCurrentStateToPrevious();
	void resumeCurrentStateFromPrevious();

	template <typename State>
	void setCurrentState();

	cocos2d::Vec2 m_TouchOneByOneBeganPosition;
	cocos2d::EventListenerTouchOneByOne * m_TouchOneByOne{ cocos2d::EventListenerTouchOneByOne::create() };

	std::weak_ptr<WarSceneScript> m_Script;

	//Controller states.
	class BaseState;
	class StateIdle;
	class StateDraggingScene;
	class StateActivatedUnit;
	class StateMakingMovePath;

	std::shared_ptr<BaseState> m_PreviousState;
	std::shared_ptr<BaseState> m_CurrentState;
	std::unordered_map<std::type_index, std::shared_ptr<BaseState>> m_States;
};

class WarSceneController::WarSceneControllerImpl::BaseState
{
public:
	virtual ~BaseState() = default;

	virtual bool onTouchOneByOneBegan(cocos2d::Touch * touch, cocos2d::Event * event)
	{
		m_ControllerImpl.lock()->m_TouchOneByOneBeganPosition = touch->getLocation();
		return true;
	}

	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) = 0;
	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) = 0;

protected:
	BaseState(std::weak_ptr<WarSceneControllerImpl> controllerImpl) : m_ControllerImpl{ std::move(controllerImpl) } {}

	std::weak_ptr<WarSceneControllerImpl> m_ControllerImpl;
};

class WarSceneController::WarSceneControllerImpl::StateIdle : public BaseState {
public:
	StateIdle(std::weak_ptr<WarSceneControllerImpl> controllerImpl) : BaseState(controllerImpl) {}
	virtual ~StateIdle() = default;

protected:
	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just starts dragging the scene, without doing any other things.
		//Just set the touch state to DraggingScene and set the position of the scene.
		auto controllerImpl = m_ControllerImpl.lock();
		controllerImpl->saveCurrentStateToPrevious();
		controllerImpl->setCurrentState<StateDraggingScene>();

		controllerImpl->queueEventDragScene(touch->getLocation() - touch->getPreviousLocation());
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just touch something without moving.
		//If he touches an unit, activate it and set the touch state to UnitActivated. Do nothing otherwise.
		auto controllerImpl = m_ControllerImpl.lock();
		auto script = controllerImpl->m_Script.lock();

		if (script->canActivateUnitAtPosition(controllerImpl->m_TouchOneByOneBeganPosition)) {
			controllerImpl->queueEventActivateUnitAtPosition(controllerImpl->m_TouchOneByOneBeganPosition);
			controllerImpl->setCurrentState<StateActivatedUnit>();
		}
	}
};

class WarSceneController::WarSceneControllerImpl::StateDraggingScene : public BaseState {
public:
	StateDraggingScene(std::weak_ptr<WarSceneControllerImpl> manager) : BaseState(manager) {}
	virtual ~StateDraggingScene() = default;

protected:
	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player continues dragging the scene, without doing any other things.
		//Just set the position of the scene.
		m_ControllerImpl.lock()->queueEventDragScene(touch->getLocation() - touch->getPreviousLocation());
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player just finishes dragging the scene.
		//Just set the touch state to the previous state and do nothing else.
		m_ControllerImpl.lock()->resumeCurrentStateFromPrevious();
	}
};

class WarSceneController::WarSceneControllerImpl::StateActivatedUnit : public BaseState {
public:
	StateActivatedUnit(std::weak_ptr<WarSceneControllerImpl> manager) : BaseState(manager) {}
	virtual ~StateActivatedUnit() = default;

protected:
	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player drag something while an unit is activated. There are some possible meanings of the drag.

		//Some variables to make the job easier.
		auto controllerImpl = m_ControllerImpl.lock();
		auto script = controllerImpl->m_Script.lock();

		if (script->isUnitActiveAtPosition(controllerImpl->m_TouchOneByOneBeganPosition)) {
			//1. The player is dragging the activated unit. It means that he's making a move path for the unit.
			//Set the touch state to DrawingMovePath and show the path as player touch.
			controllerImpl->setCurrentState<StateMakingMovePath>();

			controllerImpl->queueEventMakeMovePath(touch->getLocation());
		}
		else {
			//2. The player doesn't touch any units, or the unit he touches is not the activated unit. It means that he's dragging the scene.
			//Just set the touch state to DraggingScene and set the position of the scene.
			controllerImpl->saveCurrentStateToPrevious();
			controllerImpl->setCurrentState<StateDraggingScene>();

			controllerImpl->queueEventDragScene(touch->getLocation() - touch->getPreviousLocation());
		}
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player touched something without moving. There are some possible meanings of the touch.
		//There will be much more conditions here. For now, we just assumes that the player is to activate another unit or deactivate the active unit.
		//#TODO: Add more conditions, such as the player touched a command and so on.
		auto controllerImpl = m_ControllerImpl.lock();
		auto script = controllerImpl->m_Script.lock();
		auto touchLocation = touch->getLocation();

		if (script->isUnitActiveAtPosition(touchLocation)) {
			//The player touched the active unit. It means that he'll deactivate it.
			controllerImpl->setCurrentState<StateIdle>();
			controllerImpl->queueEventDeactivateActiveUnit();
		}
		else {
			//The player touched an inactive unit or an empty grid.
			//Activate the unit (if it exist) and set the touch state corresponding to the activate result.
			if (script->canActivateUnitAtPosition(touchLocation))
				controllerImpl->queueEventActivateUnitAtPosition(touchLocation);
			else {
				controllerImpl->setCurrentState<StateIdle>();
				controllerImpl->queueEventDeactivateActiveUnit();
			}
		}
	}
};

class WarSceneController::WarSceneControllerImpl::StateMakingMovePath : public BaseState {
public:
	StateMakingMovePath(std::weak_ptr<WarSceneControllerImpl> manager) : BaseState(manager) {}
	virtual ~StateMakingMovePath() = default;

protected:
	virtual void onTouchOneByOneMoved(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player is continuing making move path for the active unit.
		//Show the path as he touch.
		m_ControllerImpl.lock()->queueEventMakeMovePath(touch->getLocation());
	}

	virtual void onTouchOneByOneEnded(cocos2d::Touch * touch, cocos2d::Event * event) override
	{
		//The player finishes making the move path.
		auto controllerImpl = m_ControllerImpl.lock();
		controllerImpl->setCurrentState<StateIdle>();

		controllerImpl->queueEventFinishMakeMovePath(touch->getLocation());
	}
};

WarSceneController::WarSceneControllerImpl::WarSceneControllerImpl()
{
	m_TouchOneByOne->retain();
}

WarSceneController::WarSceneControllerImpl::~WarSceneControllerImpl()
{
	m_TouchOneByOne->release();
}

void WarSceneController::WarSceneControllerImpl::init(std::weak_ptr<WarSceneControllerImpl> self)
{
	//Create the state instances.
	m_States.emplace(typeid(StateIdle), std::make_shared<StateIdle>(self));
	m_States.emplace(typeid(StateDraggingScene), std::make_shared<StateDraggingScene>(self));
	m_States.emplace(typeid(StateActivatedUnit), std::make_shared<StateActivatedUnit>(self));
	m_States.emplace(typeid(StateMakingMovePath), std::make_shared<StateMakingMovePath>(self));

	setCurrentState<StateIdle>();

	//Initialize the touch listener.
	m_TouchOneByOne->onTouchBegan = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		return m_CurrentState->onTouchOneByOneBegan(touch, event);
	};
	m_TouchOneByOne->onTouchMoved = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		m_CurrentState->onTouchOneByOneMoved(touch, event);
	};
	m_TouchOneByOne->onTouchEnded = [this](cocos2d::Touch * touch, cocos2d::Event * event) {
		m_CurrentState->onTouchOneByOneEnded(touch, event);
	};
}

void WarSceneController::WarSceneControllerImpl::queueEventDragScene(const cocos2d::Vec2 & offset) const
{
	auto dragSceneEvent = std::make_unique<EvtDataDragScene>(offset);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(dragSceneEvent));
}

void WarSceneController::WarSceneControllerImpl::queueEventActivateUnitAtPosition(const cocos2d::Vec2 & pos) const
{
	auto activateUnitEvent = std::make_unique<EvtDataActivateUnitAtPosition>(pos);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(activateUnitEvent));
}

void WarSceneController::WarSceneControllerImpl::queueEventDeactivateActiveUnit() const
{
	auto deactivateUnitEvent = std::make_unique<EvtDataDeactivateActiveUnit>();
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(deactivateUnitEvent));
}

void WarSceneController::WarSceneControllerImpl::queueEventMakeMovePath(const cocos2d::Vec2 & pos) const
{
	auto makeMovePathEvent = std::make_unique<EvtDataMakeMovingPath>(pos);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(makeMovePathEvent));
}

void WarSceneController::WarSceneControllerImpl::queueEventFinishMakeMovePath(const cocos2d::Vec2 & pos) const
{
	auto makeMovePathEvent = std::make_unique<EvtDataFinishMakeMovingPath>(pos);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(makeMovePathEvent));
}

void WarSceneController::WarSceneControllerImpl::saveCurrentStateToPrevious()
{
	m_PreviousState = m_CurrentState;
}

void WarSceneController::WarSceneControllerImpl::resumeCurrentStateFromPrevious()
{
	m_CurrentState = m_PreviousState;
}

template <typename State>
void WarSceneController::WarSceneControllerImpl::setCurrentState()
{
	auto stateIter = m_States.find(typeid(State));
	assert(stateIter != m_States.end() && "WarSceneControllerImpl::setCurrentState() there's no state of the template parameter.");

	m_CurrentState = stateIter->second;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneController.
//////////////////////////////////////////////////////////////////////////
WarSceneController::WarSceneController() : pimpl{ std::make_shared<WarSceneControllerImpl>() }
{
	pimpl->init(pimpl);
}

WarSceneController::~WarSceneController()
{
	unsetTarget();
}

void WarSceneController::vSetTargetDelegate(const std::shared_ptr<Actor> & actor)
{
	auto sceneScript = actor->getComponent<WarSceneScript>();
	assert(sceneScript && "WarSceneController::setTarget() the actor has not a WarSceneScript.");
	pimpl->m_Script = sceneScript;
}

void WarSceneController::vUnsetTargetDelegate()
{
	pimpl->m_Script.reset();
}

void WarSceneController::vSetEnableDelegate(bool enable, cocos2d::Node * targetSceneNode)
{
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	if (!enable) {
		eventDispatcher->removeEventListener(pimpl->m_TouchOneByOne);
		return;
	}

	eventDispatcher->addEventListenerWithSceneGraphPriority(pimpl->m_TouchOneByOne, targetSceneNode);
}
