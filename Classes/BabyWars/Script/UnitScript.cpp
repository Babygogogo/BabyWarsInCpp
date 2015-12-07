#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataRequestChangeUnitState.h"
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

	void onRequestChangeUnitState(const EvtDataRequestChangeUnitState & e);

	bool canSetState(UnitState state) const;
	void setState(UnitState state);

	void updateAppearanceAccordingToState(UnitState state);
	void updateMoveAction(const MovingPath & path);

	//#TODO: This should be replaced to show the animation of the active unit.
	cocos2d::Action * m_ActiveAction{ nullptr };
	cocos2d::Action * m_MoveAction{ nullptr };

	std::weak_ptr<UnitScript> m_Script;
	GridIndex m_GridIndex;
	GridIndex m_IndexBeforeMoving;
	std::shared_ptr<UnitData> m_UnitData;
	UnitState m_State{ UnitState::Invalid };

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TransformComponent> m_TransformComponent;
};

UnitScript::UnitScriptImpl::UnitScriptImpl()
{
	m_ActiveAction = cocos2d::RepeatForever::create(cocos2d::Sequence::create(cocos2d::RotateTo::create(0.2f, 30, 30), cocos2d::RotateTo::create(0.4f, -30, -30), cocos2d::RotateTo::create(0.2f, 0, 0), nullptr));
	m_ActiveAction->retain();
}

UnitScript::UnitScriptImpl::~UnitScriptImpl()
{
	CC_SAFE_RELEASE_NULL(m_ActiveAction);
	CC_SAFE_RELEASE_NULL(m_MoveAction);
}

void UnitScript::UnitScriptImpl::onRequestChangeUnitState(const EvtDataRequestChangeUnitState & e)
{
	const auto newState = e.getNewState();
	if (canSetState(newState)) {
		setState(newState);
	}
}

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
	if (state == UnitState::Active) {
		m_RenderComponent->runAction(m_ActiveAction);
	}
	else {
		m_RenderComponent->stopAction(m_ActiveAction);
		m_TransformComponent.lock()->setRotation(0);
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
			strongScript->setGridIndexAndPosition(pathEndIndex);
			strongScript->setState(UnitState::MovingEnd);
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
	auto state = pimpl->m_State;
	assert(pimpl->m_State != UnitState::Invalid && "UnitScript::onInputTouch() the state of the unit is invalid.");

	if (state == UnitState::Active) {
		setState(UnitState::Idle);
		return true;
	}
	else if (state == UnitState::Idle) {
		setState(UnitState::Active);
		return true;
	}
	//else if (state == UnitState::Moving) {
	//	return false;
	//}
	//else if (state == UnitState::MovingEnd) {
	//	return false;
	//}

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
	pimpl->m_State = UnitState::Idle;
}

const std::shared_ptr<UnitData> & UnitScript::getUnitData() const
{
	return pimpl->m_UnitData;
}

std::vector<GameCommand> UnitScript::getCommands() const
{
	assert(pimpl->m_State != UnitState::Invalid && "UnitScript::getCommands() the unit is in invalid state.");
	auto commands = std::vector<GameCommand>{};

	if (pimpl->m_State == UnitState::Idle || pimpl->m_State == UnitState::Active || pimpl->m_State == UnitState::Moving || pimpl->m_State == UnitState::Waiting) {
		return commands;
	}

	if (pimpl->m_State == UnitState::MovingEnd) {
		commands.emplace_back("Wait", [unitScript = pimpl->m_Script]() {
			SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestChangeUnitState>(unitScript, UnitState::Waiting));
		});
	}

	return commands;
}

void UnitScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes and dispatch event.
	pimpl->m_GridIndex = gridIndex;
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataUnitIndexChangeEnd>(pimpl->m_Script));

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
	assert(pimpl->m_State == UnitState::Active && "UnitScript::moveAlongPath() the unit is not in active state.");
	assert(path.getLength() != 0 && "UnitScript::moveAlongPath() the length of the path is 0.");
	assert(path.getFrontNode().m_GridIndex == pimpl->m_GridIndex && "UnitScript::moveAlongPath() the unit is not at the starting grid of the path.");

	pimpl->m_IndexBeforeMoving = pimpl->m_GridIndex;
	pimpl->updateMoveAction(path);
	pimpl->m_RenderComponent->runAction(pimpl->m_MoveAction);
	setState(UnitState::Moving);
}

void UnitScript::undoMove()
{
	assert(pimpl->m_State != UnitState::Invalid && "UnitScript::undoMove() the unit is in invalid state.");
	if (pimpl->m_State == UnitState::Idle || pimpl->m_State == UnitState::Waiting)
		return;

	if (pimpl->m_State == UnitState::Moving || pimpl->m_State == UnitState::MovingEnd) {
		pimpl->m_RenderComponent->stopAction(pimpl->m_MoveAction);
		setGridIndexAndPosition(pimpl->m_IndexBeforeMoving);
	}

	setState(UnitState::Idle);
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

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataRequestChangeUnitState::s_EventType, pimpl, [](const auto & e) {
	});
}

const std::string UnitScript::Type = "UnitScript";

const std::string & UnitScript::getType() const
{
	return Type;
}
