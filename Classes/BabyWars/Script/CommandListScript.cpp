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
#include "../Event/EvtDataGameCommandGenerated.h"
#include "../Utilities/GameCommand.h"
#include "UnitScript.h"
#include "CommandListItemScript.h"
#include "CommandListScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CommandListScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CommandListScript::CommandListScriptImpl
{
	CommandListScriptImpl() = default;
	~CommandListScriptImpl() = default;

	using ItemActors = std::vector<std::weak_ptr<Actor>>;

	void onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e);
	void onGameCommandsGenerated(const EvtDataGameCommandGenerated & e);

	bool isListShownForUnit(const UnitScript & unit) const;
	void createAndShowListForUnit(const UnitScript & unit);
	void clearAndDestroyListForUnit(const UnitScript & unit);

	ItemActors _createItemActorsForGameCommands(const std::vector<GameCommand> & gameCommands) const;
	ItemActors _createItemActorsForUnit(const UnitScript & unit) const;
	void _showAndAddChildrenItemActors(const ItemActors & itemActors);
	void _hideAndRemoveChildrenItemActors(const ItemActors & itemActors);

	static std::string s_ListItemActorPath;

	std::map<const UnitScript *, ItemActors> m_ChildrenItemActorsForUnit;
	ItemActors m_ChildrenItemActors;

	std::weak_ptr<Actor> m_OwnerActor;
};

std::string CommandListScript::CommandListScriptImpl::s_ListItemActorPath;

void CommandListScript::CommandListScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	auto unitScript = e.getUnitScript();
	assert(unitScript && "CommandListScriptImpl::onUnitStateChangeEnd() the unit is expired.");
	e.getCurrentState()->vUpdateCommandList(*m_OwnerActor.lock()->getComponent<CommandListScript>(), *unitScript);
}

void CommandListScript::CommandListScriptImpl::onGameCommandsGenerated(const EvtDataGameCommandGenerated & e)
{
	_hideAndRemoveChildrenItemActors(m_ChildrenItemActors);

	if (const auto gameCommands = e.getGameCommands()) {
		m_ChildrenItemActors = _createItemActorsForGameCommands(*gameCommands);
		_showAndAddChildrenItemActors(m_ChildrenItemActors);
	}
}

bool CommandListScript::CommandListScriptImpl::isListShownForUnit(const UnitScript & unit) const
{
	return m_ChildrenItemActorsForUnit.find(&unit) != m_ChildrenItemActorsForUnit.end();
}

void CommandListScript::CommandListScriptImpl::createAndShowListForUnit(const UnitScript & unit)
{
	if (isListShownForUnit(unit)) {
		return;
	}

	auto itemActors = _createItemActorsForUnit(unit);
	_showAndAddChildrenItemActors(itemActors);
	m_ChildrenItemActorsForUnit.emplace(std::make_pair(&unit, std::move(itemActors)));
}

void CommandListScript::CommandListScriptImpl::clearAndDestroyListForUnit(const UnitScript & unit)
{
	if (!isListShownForUnit(unit)) {
		return;
	}

	auto itemActorsIter = m_ChildrenItemActorsForUnit.find(&unit);
	_hideAndRemoveChildrenItemActors(itemActorsIter->second);
	m_ChildrenItemActorsForUnit.erase(itemActorsIter);
}

CommandListScript::CommandListScriptImpl::ItemActors CommandListScript::CommandListScriptImpl::_createItemActorsForUnit(const UnitScript & unit) const
{
	return _createItemActorsForGameCommands(unit.getCommands());
}

CommandListScript::CommandListScriptImpl::ItemActors CommandListScript::CommandListScriptImpl::_createItemActorsForGameCommands(const std::vector<GameCommand> & gameCommands) const
{
	auto itemActors = ItemActors{};
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & command : gameCommands) {
		auto listItem = gameLogic->createActor(CommandListScriptImpl::s_ListItemActorPath.c_str());
		listItem->getComponent<CommandListItemScript>()->initWithGameCommand(command);

		itemActors.emplace_back(listItem);
	}

	return itemActors;
}

void CommandListScript::CommandListScriptImpl::_showAndAddChildrenItemActors(const ItemActors & itemActors)
{
	auto ownerActor = m_OwnerActor.lock();

	for (const auto & itemActor : itemActors) {
		if (!itemActor.expired()) {
			ownerActor->addChild(*itemActor.lock());
		}
	}
}

void CommandListScript::CommandListScriptImpl::_hideAndRemoveChildrenItemActors(const ItemActors & itemActors)
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	auto ownerActor = m_OwnerActor.lock();

	for (const auto & weakItemActor : itemActors) {
		if (!weakItemActor.expired()) {
			auto strongItemActor = weakItemActor.lock();
			ownerActor->removeChild(*strongItemActor);
			eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(strongItemActor->getID()));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CommandListScript.
//////////////////////////////////////////////////////////////////////////
CommandListScript::CommandListScript() : pimpl{ std::make_shared<CommandListScriptImpl>() }
{
}

CommandListScript::~CommandListScript()
{
}

void CommandListScript::showListForUnit(const UnitScript & unit)
{
	pimpl->createAndShowListForUnit(unit);
}

void CommandListScript::clearListForUnit(const UnitScript & unit)
{
	pimpl->clearAndDestroyListForUnit(unit);
}

bool CommandListScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	CommandListScriptImpl::s_ListItemActorPath = relatedActorsElement->Attribute("ListItem");

	isStaticMemberInitialized = true;
	return true;
}

void CommandListScript::vPostInit()
{
	pimpl->m_OwnerActor = m_OwnerActor;

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataUnitStateChangeEnd::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onUnitStateChangeEnd(static_cast<const EvtDataUnitStateChangeEnd &>(e));
	});
	eventDispatcher->vAddListener(EvtDataGameCommandGenerated::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onGameCommandsGenerated(static_cast<const EvtDataGameCommandGenerated &>(e));
	});
}

const std::string CommandListScript::Type{ "CommandListScript" };

const std::string & CommandListScript::getType() const
{
	return Type;
}
