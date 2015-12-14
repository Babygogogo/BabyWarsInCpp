#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/ActionComponent.h"
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
#include "../Utilities/UnitStateTypeCode.h"
#include "../Utilities/UnitStateFactory.h"
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

	bool canSetState(UnitStateTypeCode stateCode) const;
	void setState(std::shared_ptr<UnitState> && state);
	void setStateAndAppearanceAndQueueEvent(UnitStateTypeCode newStateCode);

	void updateMovingActionForPath(const MovingPath & path);
	cocos2d::Action * _createMovingActionForPath(const MovingPath & path) const;

	//#TODO: This should be replaced to show the animation of the active unit.
	cocos2d::Action * m_MovingAction{ nullptr };

	GridIndex m_GridIndex;
	GridIndex m_GridIndexBeforeMoving;
	std::shared_ptr<UnitData> m_UnitData;
	std::shared_ptr<UnitState> m_State;

	std::weak_ptr<UnitScript> m_Script;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
	std::shared_ptr<TransformComponent> m_TransformComponent;
	std::shared_ptr<ActionComponent> m_ActionComponent;
};

UnitScript::UnitScriptImpl::UnitScriptImpl() : m_State{ utilities::createUnitState(UnitStateTypeCode::Invalid) }
{
}

UnitScript::UnitScriptImpl::~UnitScriptImpl()
{
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

bool UnitScript::UnitScriptImpl::canSetState(UnitStateTypeCode stateCode) const
{
	return true;
}

void UnitScript::UnitScriptImpl::setState(std::shared_ptr<UnitState> && state)
{
	assert(state && "UnitScriptImpl::setState() the state is nullptr.");

	if (m_State->vGetStateTypeCode() != state->vGetStateTypeCode()) {
		auto unitScript = m_Script.lock();
		m_State->onUnitExitState(*unitScript);
		m_State = std::move(state);
		m_State->onUnitEnterState(*unitScript);
	}
}

void UnitScript::UnitScriptImpl::setStateAndAppearanceAndQueueEvent(UnitStateTypeCode newStateCode)
{
	if (m_State->vGetStateTypeCode() == newStateCode) {
		return;
	}

	auto newState = utilities::createUnitState(newStateCode);
	auto changeStateEvent = std::make_unique<EvtDataUnitStateChangeEnd>(m_Script, m_State, newState);
	setState(std::move(newState));
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(changeStateEvent));
}

void UnitScript::UnitScriptImpl::updateMovingActionForPath(const MovingPath & path)
{
	CC_SAFE_RELEASE_NULL(m_MovingAction);
	m_MovingAction = _createMovingActionForPath(path);
	m_MovingAction->retain();
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
			strongScript->setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::MovingEnd);
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
	return pimpl->m_State->vUpdateUnitOnTouch(*this);
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
	pimpl->m_State = utilities::createUnitState(UnitStateTypeCode::Idle);
}

const std::shared_ptr<UnitData> & UnitScript::getUnitData() const
{
	return pimpl->m_UnitData;
}

std::vector<GameCommand> UnitScript::getCommands() const
{
	return pimpl->m_State->vGetCommandsForUnit(pimpl->m_Script.lock());
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

bool UnitScript::canSetState(UnitStateTypeCode stateCode) const
{
	return pimpl->canSetState(stateCode);
}

UnitStateTypeCode UnitScript::getStateCode() const
{
	return pimpl->m_State->vGetStateTypeCode();
}

void UnitScript::setStateAndAppearanceAndQueueEvent(UnitStateTypeCode stateCode)
{
	pimpl->setStateAndAppearanceAndQueueEvent(stateCode);
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
	assert(pimpl->m_State->vCanMoveAlongPath() && "UnitScript::moveAlongPath() the unit is in a state that can't move along path.");
	assert(path.getLength() != 0 && "UnitScript::moveAlongPath() the length of the path is 0.");
	assert(path.getFrontNode().m_GridIndex == pimpl->m_GridIndex && "UnitScript::moveAlongPath() the unit is not at the starting grid of the path.");

	pimpl->updateMovingActionForPath(path);
	pimpl->m_ActionComponent->runAction(pimpl->m_MovingAction);
	setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Moving);
}

void UnitScript::moveInPlace()
{
	moveAlongPath(MovingPath(MovingPath::PathNode(pimpl->m_GridIndex, pimpl->m_UnitData->getMovementRange())));
}

void UnitScript::undoMove()
{
	if (pimpl->m_State->vCanUndoMove()) {
		pimpl->m_ActionComponent->stopAction(pimpl->m_MovingAction);
		setGridIndexAndPosition(pimpl->m_GridIndexBeforeMoving);
		setStateAndAppearanceAndQueueEvent(UnitStateTypeCode::Idle);
	}
}

bool UnitScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void UnitScript::vPostInit()
{
	pimpl->m_Script = getComponent<UnitScript>();

	pimpl->m_RenderComponent = getRenderComponent();
	assert(pimpl->m_RenderComponent && "UnitScript::vPostInit() the actor has no render component.");

	pimpl->m_TransformComponent = getComponent<TransformComponent>();
	assert(pimpl->m_TransformComponent && "UnitScript::vPostInit() the actor has no transform component.");

	pimpl->m_ActionComponent = getComponent<ActionComponent>();
	assert(pimpl->m_ActionComponent && "UnitScript::vPostInit() the actor has no action component.");

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
