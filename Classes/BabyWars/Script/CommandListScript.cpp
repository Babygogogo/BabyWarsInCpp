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

	bool isListShownForUnit(const UnitScript & unit) const;
	void createAndShowListForUnit(const UnitScript & unit);
	void clearAndDestroyListForUnit(const UnitScript & unit);

	ItemActors _createItemActorsForUnit(const UnitScript & unit) const;
	void _showAndAddChildItemActors(const ItemActors & itemActors);
	void _hideAndRemoveChildItemActors(const ItemActors & itemActors);

	static std::string s_ListItemActorPath;

	std::map<const UnitScript *, ItemActors> m_ChildrenItemActors;

	std::weak_ptr<Actor> m_OwnerActor;
};

std::string CommandListScript::CommandListScriptImpl::s_ListItemActorPath;

void CommandListScript::CommandListScriptImpl::onUnitStateChangeEnd(const EvtDataUnitStateChangeEnd & e)
{
	auto unitScript = e.getUnitScript();
	assert(unitScript && "CommandListScriptImpl::onUnitStateChangeEnd() the unit is expired.");
	e.getCurrentState()->vUpdateCommandList(*m_OwnerActor.lock()->getComponent<CommandListScript>(), *unitScript);
}

bool CommandListScript::CommandListScriptImpl::isListShownForUnit(const UnitScript & unit) const
{
	return m_ChildrenItemActors.find(&unit) != m_ChildrenItemActors.end();
}

void CommandListScript::CommandListScriptImpl::createAndShowListForUnit(const UnitScript & unit)
{
	if (isListShownForUnit(unit)) {
		return;
	}

	auto itemActors = _createItemActorsForUnit(unit);
	_showAndAddChildItemActors(itemActors);
	m_ChildrenItemActors.emplace(std::make_pair(&unit, std::move(itemActors)));
}

void CommandListScript::CommandListScriptImpl::clearAndDestroyListForUnit(const UnitScript & unit)
{
	if (!isListShownForUnit(unit)) {
		return;
	}

	auto itemActorsIter = m_ChildrenItemActors.find(&unit);
	_hideAndRemoveChildItemActors(itemActorsIter->second);
	m_ChildrenItemActors.erase(itemActorsIter);
}

CommandListScript::CommandListScriptImpl::ItemActors CommandListScript::CommandListScriptImpl::_createItemActorsForUnit(const UnitScript & unit) const
{
	auto itemActors = ItemActors{};
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (const auto & command : unit.getCommands()) {
		auto listItem = gameLogic->createActor(CommandListScriptImpl::s_ListItemActorPath.c_str());
		listItem->getComponent<CommandListItemScript>()->initWithGameCommand(command);

		itemActors.emplace_back(listItem);
	}

	return itemActors;
}

void CommandListScript::CommandListScriptImpl::_showAndAddChildItemActors(const ItemActors & itemActors)
{
	auto ownerActor = m_OwnerActor.lock();
	auto listView = static_cast<cocos2d::ui::ListView*>(ownerActor->getRenderComponent()->getSceneNode());

	for (const auto & itemActor : itemActors) {
		if (itemActor.expired()) {
			continue;
		}

		auto strongItemActor = itemActor.lock();
		ownerActor->addChild(*strongItemActor);
		//listView->pushBackCustomItem(static_cast<cocos2d::ui::Widget*>(strongItemActor->getRenderComponent()->getSceneNode()));
	}
}

void CommandListScript::CommandListScriptImpl::_hideAndRemoveChildItemActors(const ItemActors & itemActors)
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();

	for (const auto & weakItemActor : itemActors) {
		if (weakItemActor.expired()) {
			continue;
		}

		auto strongItemActor = weakItemActor.lock();
		strongItemActor->removeFromParent();
		strongItemActor->getRenderComponent()->getSceneNode()->removeFromParent();
		eventDispatcher->vQueueEvent(std::make_unique<EvtDataRequestDestroyActor>(strongItemActor->getID()));
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
}

const std::string CommandListScript::Type{ "CommandListScript" };

const std::string & CommandListScript::getType() const
{
	return Type;
}
