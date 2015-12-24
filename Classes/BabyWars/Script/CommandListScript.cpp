#include <vector>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Event/EvtDataRequestDestroyActor.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataGameCommandGenerated.h"
#include "../Utilities/GameCommand.h"
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

	void onGameCommandsGenerated(const EvtDataGameCommandGenerated & e);

	ItemActors _createItemActorsForGameCommands(const std::vector<GameCommand> & gameCommands) const;
	void _showAndAddChildrenItemActors(const ItemActors & itemActors);
	void _hideAndRemoveChildrenItemActors(const ItemActors & itemActors);

	static std::string s_ListItemActorPath;

	ItemActors m_ChildrenItemActors;

	std::weak_ptr<Actor> m_OwnerActor;
};

std::string CommandListScript::CommandListScriptImpl::s_ListItemActorPath;

void CommandListScript::CommandListScriptImpl::onGameCommandsGenerated(const EvtDataGameCommandGenerated & e)
{
	_hideAndRemoveChildrenItemActors(m_ChildrenItemActors);
	m_ChildrenItemActors = _createItemActorsForGameCommands(e.getGameCommands());
	_showAndAddChildrenItemActors(m_ChildrenItemActors);
}

CommandListScript::CommandListScriptImpl::ItemActors CommandListScript::CommandListScriptImpl::_createItemActorsForGameCommands(const std::vector<GameCommand> & gameCommands) const
{
	auto itemActors = ItemActors{};
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & command : gameCommands) {
		auto listItem = gameLogic->createActorAndChildren(CommandListScriptImpl::s_ListItemActorPath.c_str());
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
	eventDispatcher->vAddListener(EvtDataGameCommandGenerated::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onGameCommandsGenerated(static_cast<const EvtDataGameCommandGenerated &>(e));
	});
}

const std::string CommandListScript::Type{ "CommandListScript" };

const std::string & CommandListScript::getType() const
{
	return Type;
}
