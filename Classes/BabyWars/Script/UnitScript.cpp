#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/SpriteRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Resource/ResourceLoader.h"
#include "../Resource/UnitData.h"
#include "../Utilities/GridIndex.h"
#include "../Utilities/MovingPath.h"
#include "../Utilities/UnitState.h"
#include "UnitScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of UnitScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct UnitScript::UnitScriptImpl
{
	UnitScriptImpl();
	~UnitScriptImpl();

	void updateAppearanceAccordingToState(UnitState state);

	//#TODO: This should be replaced to show the animation of the activate unit.
	cocos2d::Action * m_ActiveAction{ nullptr };

	std::weak_ptr<const UnitScript> m_Script;
	GridIndex m_GridIndex;
	std::shared_ptr<UnitData> m_UnitData;
	UnitState m_State{ UnitState::Invalid };

	std::weak_ptr<SpriteRenderComponent> m_SpriteRenderComponent;
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
}

void UnitScript::UnitScriptImpl::updateAppearanceAccordingToState(UnitState state)
{
	auto spriteRenderComponent = m_SpriteRenderComponent.lock();
	if (state == UnitState::Active) {
		spriteRenderComponent->runAction(m_ActiveAction);
	}
	else {
		spriteRenderComponent->stopAction(m_ActiveAction);
		m_TransformComponent.lock()->setRotation(0);
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of WorldScript.
//////////////////////////////////////////////////////////////////////////
UnitScript::UnitScript() : pimpl{ std::make_unique<UnitScriptImpl>() }
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
	//#TODO: This only shows the first first frame of the animation. Update the code to show the whole animation.
	pimpl->m_SpriteRenderComponent.lock()->setSpriteFrame(unitData->getAnimation()->getFrames().at(0)->getSpriteFrame());

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

void UnitScript::setGridIndexAndPosition(const GridIndex & gridIndex)
{
	//Set the indexes.
	pimpl->m_GridIndex = gridIndex;

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
	return true;
}

UnitState UnitScript::getState() const
{
	return pimpl->m_State;
}

void UnitScript::setState(UnitState state)
{
	if (pimpl->m_State == state) {
		return;
	}

	auto changeStateEvent = std::make_unique<EvtDataUnitStateChangeEnd>(pimpl->m_Script, pimpl->m_State, state);
	pimpl->m_State = state;
	pimpl->updateAppearanceAccordingToState(state);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(changeStateEvent));
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
	assert(path.getLength() > 1 && "UnitScript::moveAlongPath() the length of the path <= 1.");
	assert(path.getFrontNode().m_GridIndex == pimpl->m_GridIndex && "UnitScript::moveAlongPath() the unit is not at the starting grid of the path.");
	assert(path.getFrontNode().m_GridIndex != path.getBackNode().m_GridIndex && "UnitScript::moveAlongPath() the starting and ending grid of the path is the same.");

	pimpl->m_GridIndex = path.getBackNode().m_GridIndex;

	auto gridSize = SingletonContainer::getInstance()->get<ResourceLoader>()->getDesignGridSize();
	auto movingSpeed = pimpl->m_UnitData->getAnimationMovingSpeed();
	auto movingTimePerGrid = gridSize.width / movingSpeed;

	const auto & underlyingPath = path.getUnderlyingPath();
	auto moveActionList = cocos2d::Vector<cocos2d::FiniteTimeAction*>();
	for (auto i = 1u; i < underlyingPath.size(); ++i)
		moveActionList.pushBack(cocos2d::MoveTo::create(movingTimePerGrid, underlyingPath[i].m_GridIndex.toPosition(gridSize)));

	auto moveSequence = cocos2d::Sequence::create(moveActionList);
	pimpl->m_SpriteRenderComponent.lock()->runAction(moveSequence);
}

bool UnitScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void UnitScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	pimpl->m_Script = ownerActor->getComponent<UnitScript>();

	auto renderComponent = ownerActor->getRenderComponent<SpriteRenderComponent>();
	assert(renderComponent && "UnitScript::vPostInit() the actor has no sprite render component.");
	pimpl->m_SpriteRenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "UnitScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);
}

const std::string UnitScript::Type = "UnitScript";

const std::string & UnitScript::getType() const
{
	return Type;
}
