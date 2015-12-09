#include <vector>

#include "ui/UIListView.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataUnitStateChangeEnd.h"
#include "../Utilities/GameCommand.h"
#include "UnitScript.h"
#include "CommandListItemScript.h"
#include "ActionListScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionListScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionListScript::ActionListScriptImpl
{
	ActionListScriptImpl() = default;
	~ActionListScriptImpl() = default;

	void onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e);

	bool isListShownForUnit(const std::shared_ptr<UnitScript> & unit) const;
	void showListForUnit(const std::shared_ptr<UnitScript> & unit);
	void clearListForUnit();

	static std::string s_ListItemActorPath;

	std::weak_ptr<Actor> m_OwnerActor;
	std::vector<std::weak_ptr<Actor>> m_ChildrenItemActors;

	std::weak_ptr<UnitScript> m_FocusedUnit;
	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

void ActionListScript::ActionListScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	using State = UnitState::State;
	const auto currentState = e.getCurrentState().getState();
	if (currentState == State::Idle) {
		if (isListShownForUnit(e.getUnitScript())) {
			clearListForUnit();
		}
	}
	else if (currentState == State::Moving) {
		clearListForUnit();
	}
	else if (currentState == State::MovingEnd) {
		clearListForUnit();
		showListForUnit(e.getUnitScript());
	}
	else if (currentState == State::Waiting) {
		clearListForUnit();
	}
}

bool ActionListScript::ActionListScriptImpl::isListShownForUnit(const std::shared_ptr<UnitScript> & unit) const
{
	if (m_FocusedUnit.expired()) {
		return false;
	}

	return m_FocusedUnit.lock() == unit;
}

void ActionListScript::ActionListScriptImpl::showListForUnit(const std::shared_ptr<UnitScript> & unit)
{
	if (!unit) {
		return;
	}

	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto ownerActor = m_OwnerActor.lock();
	auto listView = static_cast<cocos2d::ui::ListView*>(m_RenderComponent.lock()->getSceneNode());

	for (const auto & command : unit->getCommands()) {
		auto listItem = gameLogic->createActor(ActionListScriptImpl::s_ListItemActorPath.c_str());
		listItem->getComponent<CommandListItemScript>()->initWithGameCommand(command);

		ownerActor->addChild(*listItem);
		m_ChildrenItemActors.emplace_back(listItem);
		listView->pushBackCustomItem(static_cast<cocos2d::ui::Widget*>(listItem->getRenderComponent()->getSceneNode()));
	}

	m_FocusedUnit = unit;
}

void ActionListScript::ActionListScriptImpl::clearListForUnit()
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	for (const auto & weakChild : m_ChildrenItemActors) {
		if (weakChild.expired()) {
			continue;
		}

		auto strongChild = weakChild.lock();
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(strongChild->getID()));
		strongChild->getRenderComponent()->getSceneNode()->removeFromParent();
	}

	m_ChildrenItemActors.clear();
	m_FocusedUnit.reset();
}

std::string ActionListScript::ActionListScriptImpl::s_ListItemActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionListScript.
//////////////////////////////////////////////////////////////////////////
ActionListScript::ActionListScript() : pimpl{ std::make_shared<ActionListScriptImpl>() }
{
}

ActionListScript::~ActionListScript()
{
}

bool ActionListScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	ActionListScriptImpl::s_ListItemActorPath = relatedActorsElement->Attribute("ListItem");

	isStaticMemberInitialized = true;
	return true;
}

void ActionListScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;
	auto ownerActor = m_OwnerActor.lock();

	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "ActionListScript::vPostInit() the actor has no render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataUnitStateChangeEnd::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onUnitStateChangeEnd(static_cast<const EvtDataUnitStateChangeEnd &>(e));
	});
}

const std::string ActionListScript::Type{ "ActionListScript" };

const std::string & ActionListScript::getType() const
{
	return Type;
}
