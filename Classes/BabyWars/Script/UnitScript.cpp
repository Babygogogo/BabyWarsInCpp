#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
//#include "../Event/EvtDataRequestChangeUnitState.h"
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Event/EvtDataUnitIndexChangeEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/UnitData.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/MovingPath.h"
#include "../Utilities/UnitState.h"
#include "../Utilities/GameCommand.h"
#include "UnitScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitScript::UnitScriptImpl
{
	UnitScriptImpl();
	~UnitScriptImpl();

	//void onRequestChangeUnitState(const EvtDataRequestChangeUnitState & e);

	bool canSetState(UnitState state) const;
	void setStateAndAppearanceAndQueueEvent(UnitState newState);

	void updateMovingActionForPath(const MovingPath & path);
	cocos2d::Action * _createMovingActionForPath(const MovingPath & path) const;

	//#TODO: This should be replaced to show the animation of the active unit.
	cocos2d::Action * m_ActiveAction{ nullptr };
	cocos2d::Action * m_MoveEndAction{ nullptr };
	cocos2d::Action * m_MovingAction{ nullptr };

	GridIndex m_GridIndex;
	GridIndex m_GridIndexBeforeMoving;
	std::shared_ptr<UnitData> m_UnitData;
	UnitState m_State;

	std::weak_ptr<UnitScript> m_Script;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
	std::shared_ptr<TransformComponent> m_TransformComponent;
};

UnitScript::UnitScriptImpl::UnitScriptImpl()
{
	using namespace cocos2d;
	m_ActiveAction = RepeatForever::create(Sequence::create(RotateTo::create(0.2f, 30, 30), RotateTo::create(0.4f, -30, -30), RotateTo::create(0.2f, 0, 0), nullptr));
	m_ActiveAction->retain();

	m_MoveEndAction = RepeatForever::create(Sequence::create(FadeTo::create(0.4f, 100), FadeTo::create(0.4f, 255), nullptr));
	m_MoveEndAction->retain();
}

UnitScript::UnitScriptImpl::~UnitScriptImpl()
{
	CC_SAFE_RELEASE_NULL(m_ActiveAction);
	CC_SAFE_RELEASE_NULL(m_MoveEndAction);
	CC_SAFE_RELEASE_NULL(m_MovingAction);
}

//void UnitScript::UnitScriptImpl::onRequestChangeUnitState(const EvtDataRequestChangeUnitState & e)
//{
//	if (m_Script.expired() || m_Script.lock() != e.getUnitScript()) {
//		return;
//	}
//
//	const auto newState = e.getNewState();
//	if (canSetState(newState)) {
//		setStateAndAppearanceAndQueueEvent(newState);
//	}
//}

bool UnitScript::UnitScriptImpl::canSetState(UnitState state) const
{
	return true;
}

void UnitScript::UnitScriptImpl::setStateAndAppearanceAndQueueEvent(UnitState newState)
{
	if (m_State == newState) {
		return;
	}

	auto changeStateEvent = std::make_unique<EvtDataUnitStateChangeEnd>(m_Script, m_State, newState);
	m_State.clearUnitAppearanceInState(*m_Script.lock());
	newState.showUnitAppearanceInState(*m_Script.lock());
	m_State = newState;
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(changeStateEvent));
}

void UnitScript::UnitScriptImpl::updateMovingActionForPath(const MovingPath & path)
{
	CC_SAFE_RELEASE_NULL(m_MovingAction);
	m_MovingAction = _createMovingActionForPath(path);
}

cocos2d::Action * UnitScript::UnitScriptImpl::_createMovingActionForPath(const MovingPath & path) const
{
	const auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	const auto movingDurationPerGrid = 1 / m_UnitData->getAnimationMovingSpeedGridPerSec();
	const auto & underlyingPath = path.getUnderlyingPath();

	auto moveActionList = cocos2d::Vector<cocos2d::FiniteTimeAction*>();
	for (auto i = 1u; i < underlyingPath.size(); ++i)
		moveActionList.pushBack(cocos2d::MoveTo::create(movingDurationPerGrid, underlyingPath[i].m_GridIndex.toPosition(gridSize)));

	moveActionList.pushBack(cocos2d::CallFunc::create([script = m_Script, pathEndIndex = path.getBackNode().m_GridIndex]() {
		if (!script.expired()) {
			auto strongScript = script.lock();
			strongScript->pimpl->m_GridIndex = pathEndIndex;
			strongScript->setStateAndAppearanceAndQueueEvent(UnitState(UnitState::State::MovingEnd));
		}
	}));

	return cocos2d::Sequence::create(moveActionList);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
UnitScript::UnitScript() : pimpl{ std::make_shared<UnitScriptImpl>() }
{
}

UnitScript::~UnitScript()
{
}

bool UnitScript::onInputTouch(const EvtDataInputTouch & touch)
{
	return pimpl->m_State.updateUnitOnTouch(*this);
}

void UnitScript::loadUnit(tinyxml2::XMLElement * xmlElement)
{
	//////////////////////////////////////////////////////////////////////////
	//Load and set the unit data.
	auto unitDataID = xmlElement->IntAttribute("UnitDataID");
	const auto resourceLoader = SingletonContainer::getInstance()->get<ResourceLoader>();
	auto unitData = resourceLoader->getUnitData(unitDataID);
	assert(unitData && "UnitScript::setUnitData() with nullptr.");

	auto ownerActor = m_OwnerActor.lock();
	//#TODO: This only shows the first frame of the animation. Update the code to show the whole animation.
	auto sceneNode = static_cast<cocos2d::Sprite*>(pimpl->m_RenderComponent->getSceneNode());
	sceneNode->setSpriteFrame(unitData->getAnimation()->getFrames().at(0)->getSpriteFrame());

	//Scale the sprite so that it meets the real game grid size.
	pimpl->m_TransformComponent->setScaleToSize(resourceLoader->getDesignGridSize());

	pimpl->m_UnitData = std::move(unitData);

	//////////////////////////////////////////////////////////////////////////
	//#TODO: Load more data, such as the hp, level and so on, from the xml.
	pimpl->m_State = UnitState(UnitState::State::Idle);
}

const std::shared_ptr<UnitData> & UnitScript::getUnitData() const
{
	return pimpl->m_UnitData;
}

std::vector<GameCommand> UnitScript::getCommands() const
{
	return pimpl->m_State.getCommandsForUnit(pimpl->m_Script.lock());
}

void UnitScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes and dispatch event.
	auto previousIndex = pimpl->m_GridIndex;
	pimpl->m_GridIndexBeforeMoving = pimpl->m_GridIndex = gridIndex;
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataUnitIndexChangeEnd>(pimpl->m_Script, std::move(previousIndex)));

	//Set the position of the node according to indexes.
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_TransformComponent->setPosition(gridIndex.toPosition(gridSize));
}

GridIndex UnitScript::getGridIndex() const
{
	return pimpl->m_GridIndex;
}

bool UnitScript::canSetState(UnitState state) const
{
	return pimpl->canSetState(state);
}

UnitState UnitScript::getState() const
{
	return pimpl->m_State;
}

void UnitScript::setStateAndAppearanceAndQueueEvent(UnitState state)
{
	pimpl->setStateAndAppearanceAndQueueEvent(state);
}

bool UnitScript::canPassThrough(const UnitScript & otherUnit) const
{
	//#TODO: Add more conditions here. Check if the units are allied for example.
	return true;
}

bool UnitScript::canStayTogether(const UnitScript & otherUnit) const
{
	//#TODO: Add more conditions here. Check if the units belong to the same player and are the same type for example.
	return false;
}

void UnitScript::moveAlongPath(const MovingPath & path)
{
	assert(pimpl->m_State.canMoveAlongPath() && "UnitScript::moveAlongPath() the unit is in a state that can't move along path.");
	assert(path.getLength() != 0 && "UnitScript::moveAlongPath() the length of the path is 0.");
	assert(path.getFrontNode().m_GridIndex == pimpl->m_GridIndex && "UnitScript::moveAlongPath() the unit is not at the starting grid of the path.");

	pimpl->updateMovingActionForPath(path);
	setStateAndAppearanceAndQueueEvent(UnitState(UnitState::State::Moving));
}

void UnitScript::moveInPlace()
{
	moveAlongPath(MovingPath(MovingPath::PathNode(pimpl->m_GridIndex, pimpl->m_UnitData->getMovementRange())));
}

void UnitScript::undoMove()
{
	if (pimpl->m_State.canUndoMove()) {
		setStateAndAppearanceAndQueueEvent(UnitState(UnitState::State::Idle));
		setGridIndexAndPosition(pimpl->m_GridIndexBeforeMoving);
	}
}

void UnitScript::showAppearanceInActiveState()
{
	pimpl->m_RenderComponent->runAction(pimpl->m_ActiveAction);
}

void UnitScript::clearAppearanceInActiveState()
{
	pimpl->m_RenderComponent->stopAction(pimpl->m_ActiveAction);
	pimpl->m_TransformComponent->setRotation(0);
}

void UnitScript::showAppearanceInMovingState()
{
	pimpl->m_RenderComponent->runAction(pimpl->m_MovingAction);
}

void UnitScript::clearAppearanceInMovingState()
{
	pimpl->m_RenderComponent->stopAction(pimpl->m_MovingAction);
}

void UnitScript::showAppearanceInMovingEndState()
{
	pimpl->m_RenderComponent->runAction(pimpl->m_MoveEndAction);
}

void UnitScript::clearAppearanceInMovingEndState()
{
	pimpl->m_RenderComponent->stopAction(pimpl->m_MoveEndAction);
	pimpl->m_RenderComponent->getSceneNode()->setOpacity(255);
}

void UnitScript::showAppearanceInWaitingState()
{
	pimpl->m_RenderComponent->getSceneNode()->setColor(cocos2d::Color3B(150, 150, 150));
}

void UnitScript::clearAppearanceInWaitingState()
{
	pimpl->m_RenderComponent->getSceneNode()->setColor(cocos2d::Color3B(255, 255, 255));
}

bool UnitScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void UnitScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	pimpl->m_Script = ownerActor->getComponent<UnitScript>();

	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "UnitScript::vPostInit() the actor has no sprite render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "UnitScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	//auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	//eventDispatcher->vAddListener(EvtDataRequestChangeUnitState::s_EventType, pimpl, [this](const auto & e) {
	//	pimpl->onRequestChangeUnitState(static_cast<const EvtDataRequestChangeUnitState &>(e));
	//});
}

const std::string UnitScript::Type = "UnitScript";

const std::string & UnitScript::getType() const
{
	return Type;
}
