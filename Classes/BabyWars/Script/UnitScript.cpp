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
	void setState(UnitState state);

	void updateAppearanceAccordingToState(UnitState state);
	void updateMoveAction(const MovingPath & path);

	//#TODO: This should be replaced to show the animation of the active unit.
	cocos2d::Action * m_ActiveAction{ nullptr };
	cocos2d::Action * m_MoveEndAction{ nullptr };
	cocos2d::Action * m_MoveAction{ nullptr };

	GridIndex m_GridIndex;
	GridIndex m_IndexBeforeMoving;
	std::shared_ptr<UnitData> m_UnitData;
	UnitState m_State;

	std::weak_ptr<UnitScript> m_Script;
	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TransformComponent> m_TransformComponent;
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
	CC_SAFE_RELEASE_NULL(m_MoveAction);
}

//void UnitScript::UnitScriptImpl::onRequestChangeUnitState(const EvtDataRequestChangeUnitState & e)
//{
//	if (m_Script.expired() || m_Script.lock() != e.getUnitScript()) {
//		return;
//	}
//
//	const auto newState = e.getNewState();
//	if (canSetState(newState)) {
//		setState(newState);
//	}
//}

bool UnitScript::UnitScriptImpl::canSetState(UnitState state) const
{
	return true;
}

void UnitScript::UnitScriptImpl::setState(UnitState state)
{
	if (m_State == state) {
		return;
	}

	auto changeStateEvent = std::make_unique<EvtDataUnitStateChangeEnd>(m_Script, m_State, state);
	m_State = state;
	updateAppearanceAccordingToState(state);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(changeStateEvent));
}

void UnitScript::UnitScriptImpl::updateAppearanceAccordingToState(UnitState state)
{
	//Reset the appearance.
	auto sceneNode = m_RenderComponent->getSceneNode();
	sceneNode->setColor(cocos2d::Color3B(255, 255, 255));
	m_RenderComponent->stopAction(m_ActiveAction);
	m_TransformComponent.lock()->setRotation(0);
	m_RenderComponent->stopAction(m_MoveEndAction);
	sceneNode->setOpacity(255);

	using State = UnitState::State;
	if (state.getState() == State::Active) {
		m_RenderComponent->runAction(m_ActiveAction);
	}
	else if (state.getState() == State::MovingEnd) {
		m_RenderComponent->runAction(m_MoveEndAction);
	}
	else if (state.getState() == State::Waiting) {
		m_RenderComponent->getSceneNode()->setColor(cocos2d::Color3B(150, 150, 150));
	}
}

void UnitScript::UnitScriptImpl::updateMoveAction(const MovingPath & path)
{
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	auto movingDurationPerGrid = 1 / m_UnitData->getAnimationMovingSpeedGridPerSec();
	const auto & underlyingPath = path.getUnderlyingPath();

	auto moveActionList = cocos2d::Vector<cocos2d::FiniteTimeAction*>();
	for (auto i = 1u; i < underlyingPath.size(); ++i)
		moveActionList.pushBack(cocos2d::MoveTo::create(movingDurationPerGrid, underlyingPath[i].m_GridIndex.toPosition(gridSize)));

	moveActionList.pushBack(cocos2d::CallFunc::create([script = m_Script, pathEndIndex = path.getBackNode().m_GridIndex]() {
		if (!script.expired()) {
			auto strongScript = script.lock();
			strongScript->pimpl->m_GridIndex = pathEndIndex;
			//strongScript->setGridIndexAndPosition(pathEndIndex);
			strongScript->setState(UnitState(UnitState::State::MovingEnd));
		}
	}));

	CC_SAFE_RELEASE_NULL(m_MoveAction);
	m_MoveAction = cocos2d::Sequence::create(moveActionList);
	m_MoveAction->retain();
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
	auto state = pimpl->m_State.getState();
	using State = UnitState::State;
	assert(state != State::Invalid && "UnitScript::onInputTouch() the state of the unit is invalid.");

	if (state == State::Idle) {
		setState(UnitState(State::Active));
		return true;
	}
	else if (state == State::Active) {
		moveAlongPath(MovingPath(MovingPath::PathNode(pimpl->m_GridIndex, pimpl->m_UnitData->getMovementRange())));
		return true;
	}
	else if (state == State::Moving) {
		assert("UnitScript::onInputTouch() the unit is touched when moving. Moving unit is designed to ignore any touch, therefore this should not happen.");
		return false;
	}
	else if (state == State::MovingEnd) {
		return false;
	}
	else if (state == State::Waiting) {
		return false;
	}

	return false;
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
	pimpl->m_TransformComponent.lock()->setScaleToSize(resourceLoader->getDesignGridSize());

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
	auto state = pimpl->m_State.getState();
	using State = UnitState::State;
	assert(state != State::Invalid && "UnitScript::getCommands() the unit is in invalid state.");
	auto commands = std::vector<GameCommand>{};

	if (state == State::Idle || state == State::Active || state == State::Moving || state == State::Waiting) {
		return commands;
	}

	if (state == State::MovingEnd) {
		commands.emplace_back("Wait", [unitScript = pimpl->m_Script]() {
			if (unitScript.expired()) {
				return;
			}

			auto strongUnit = unitScript.lock();
			strongUnit->setState(UnitState(State::Waiting));
			strongUnit->setGridIndexAndPosition(strongUnit->getGridIndex());
		});
	}

	return commands;
}

void UnitScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes and dispatch event.
	auto previousIndex = pimpl->m_GridIndex;
	pimpl->m_GridIndex = gridIndex;
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataUnitIndexChangeEnd>(pimpl->m_Script, std::move(previousIndex)));

	//Set the position of the node according to indexes.
	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	pimpl->m_TransformComponent.lock()->setPosition(gridIndex.toPosition(gridSize));
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

void UnitScript::setState(UnitState state)
{
	pimpl->setState(state);
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
	assert(pimpl->m_State.getState() == UnitState::State::Active && "UnitScript::moveAlongPath() the unit is not in active state.");
	assert(path.getLength() != 0 && "UnitScript::moveAlongPath() the length of the path is 0.");
	assert(path.getFrontNode().m_GridIndex == pimpl->m_GridIndex && "UnitScript::moveAlongPath() the unit is not at the starting grid of the path.");

	pimpl->m_IndexBeforeMoving = pimpl->m_GridIndex;
	pimpl->updateMoveAction(path);
	setState(UnitState(UnitState::State::Moving));
	pimpl->m_RenderComponent->runAction(pimpl->m_MoveAction);
}

void UnitScript::undoMove()
{
	auto state = pimpl->m_State.getState();
	using State = UnitState::State;
	assert(state != State::Invalid && "UnitScript::undoMove() the unit is in invalid state.");
	if (state == State::Idle || state == State::Waiting)
		return;

	if (state == State::Moving || state == State::MovingEnd) {
		pimpl->m_RenderComponent->stopAction(pimpl->m_MoveAction);
		setGridIndexAndPosition(pimpl->m_IndexBeforeMoving);
	}

	setState(UnitState(State::Idle));
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
