#include <vector>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataBeginTurn.h"
#include "../Event/EvtDataTurnEnded.h"
#include "../Event/EvtDataAllUnitsUnfocused.h"
#include "../Event/EvtDataBeginTurnEffectDisappeared.h"
#include "../Event/EvtDataGameCommandGenerated.h"
#include "../Utilities/GameCommand.h"
#include "../Utilities/PlayerID.h"
#include "PlayerManagerScript.h"
#include "TurnManagerScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TurnManagerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TurnManagerScript::TurnManagerScriptImpl
{
	TurnManagerScriptImpl() = default;
	~TurnManagerScriptImpl() = default;

	void onTurnEnded(const EvtDataTurnEnded & e);
	void onBeginTurnEffectDisappeared(const EvtDataBeginTurnEffectDisappeared & e);
	void onAllUnitsUnfocused(const EvtDataAllUnitsUnfocused & e);

	bool isTurnIndexValid() const;
	bool isPlayerIdValid() const;

	bool hasNextPlayerID(PlayerID currentPlayerID) const;
	PlayerID getNextPlayerID(PlayerID currentPlayerID) const;
	PlayerID getFirstPlayerID() const;
	int getNextTurnIndex(int currentTurnIndex) const;

	void initAsFirstTurnForFirstPlayer();

	std::vector<GameCommand> generateGameCommands() const;

	int m_CurrentTurnIndex{};
	PlayerID m_CurrentPlayerID{ INVALID_PLAYER_ID };

	std::weak_ptr<PlayerManagerScript> m_PlayerManagerScript;
};

void TurnManagerScript::TurnManagerScriptImpl::onTurnEnded(const EvtDataTurnEnded & e)
{
	assert(m_CurrentPlayerID == e.getPlayerID() && "TurnManagerScriptImpl::onTurnEnded() the player id is not the same.");
	assert(m_CurrentTurnIndex == e.getTurnIndex() && "TurnManagerScriptImpl::onTurnEnded() the turn index is not the same.");

	if (hasNextPlayerID(m_CurrentPlayerID)) {
		m_CurrentPlayerID = getNextPlayerID(m_CurrentPlayerID);
	}
	else {
		m_CurrentPlayerID = getFirstPlayerID();
		m_CurrentTurnIndex = getNextTurnIndex(m_CurrentTurnIndex);
	}

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vQueueEvent(std::make_unique<EvtDataGameCommandGenerated>(std::vector<GameCommand>{}));

	auto beginTurnEvent = std::make_unique<EvtDataBeginTurn>(m_CurrentTurnIndex, m_CurrentPlayerID);
	eventDispatcher->vQueueEvent(std::move(beginTurnEvent));
}

void TurnManagerScript::TurnManagerScriptImpl::onBeginTurnEffectDisappeared(const EvtDataBeginTurnEffectDisappeared & e)
{
	auto commandGeneratedEvent = std::make_unique<EvtDataGameCommandGenerated>(generateGameCommands());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(commandGeneratedEvent));
}

void TurnManagerScript::TurnManagerScriptImpl::onAllUnitsUnfocused(const EvtDataAllUnitsUnfocused & e)
{
	auto commandGeneratedEvent = std::make_unique<EvtDataGameCommandGenerated>(generateGameCommands());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(commandGeneratedEvent));
}

bool TurnManagerScript::TurnManagerScriptImpl::isTurnIndexValid() const
{
	return m_CurrentTurnIndex > 0;
}

bool TurnManagerScript::TurnManagerScriptImpl::isPlayerIdValid() const
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::isPlayerIdValid() the player manager script is expired or not set.");
	return m_PlayerManagerScript.lock()->hasPlayerID(m_CurrentPlayerID);
}

bool TurnManagerScript::TurnManagerScriptImpl::hasNextPlayerID(PlayerID currentPlayerID) const
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::hasNextPlayerID() the player manager is expired.");
	return m_PlayerManagerScript.lock()->hasPlayerNextOfID(currentPlayerID);
}

PlayerID TurnManagerScript::TurnManagerScriptImpl::getNextPlayerID(PlayerID currentPlayerID) const
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::getNextPlayerID() the player manager is expired.");
	return m_PlayerManagerScript.lock()->getNextPlayerID(currentPlayerID);
}

PlayerID TurnManagerScript::TurnManagerScriptImpl::getFirstPlayerID() const
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::getFirstPlayerID() the player manager is expired.");
	return m_PlayerManagerScript.lock()->getFirstPlayerID();
}

int TurnManagerScript::TurnManagerScriptImpl::getNextTurnIndex(int currentTurnIndex) const
{
	return currentTurnIndex + 1;
}

void TurnManagerScript::TurnManagerScriptImpl::initAsFirstTurnForFirstPlayer()
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::initAsFirstTurnForFirstPlayer() the player manager script is expired or not set.");

	m_CurrentTurnIndex = 1;
	m_CurrentPlayerID = m_PlayerManagerScript.lock()->getFirstPlayerID();
}

std::vector<GameCommand> TurnManagerScript::TurnManagerScriptImpl::generateGameCommands() const
{
	auto commands = std::vector<GameCommand>{};

	commands.emplace_back("End Turn", [turnIndex = m_CurrentTurnIndex, playerID = m_CurrentPlayerID]() {
		auto turnEndedEvent = std::make_unique<EvtDataTurnEnded>(turnIndex, playerID);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(turnEndedEvent));
	});

	return commands;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TurnManagerScript.
//////////////////////////////////////////////////////////////////////////
TurnManagerScript::TurnManagerScript() : pimpl{ std::make_shared<TurnManagerScriptImpl>() }
{
}

TurnManagerScript::~TurnManagerScript()
{
}

void TurnManagerScript::setPlayerManager(const std::shared_ptr<PlayerManagerScript> & playerManagerScript)
{
	pimpl->m_PlayerManagerScript = playerManagerScript;
}

void TurnManagerScript::loadTurn(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "TurnManagerScript::loadTurn() the xml element is nullptr.");

	const auto currentTurnElement = xmlElement->FirstChildElement("CurrentTurn");
	pimpl->m_CurrentTurnIndex = currentTurnElement->IntAttribute("TurnIndex");
	pimpl->m_CurrentPlayerID = currentTurnElement->IntAttribute("PlayerID");
}

void TurnManagerScript::run()
{
	if (!pimpl->isTurnIndexValid()) {
		pimpl->initAsFirstTurnForFirstPlayer();
	}

	assert(pimpl->isTurnIndexValid() && "TurnManagerScript::run() the turn index is invalid.");
	assert(pimpl->isPlayerIdValid() && "TurnManagerScript::run() the player id is invalid.");

	auto beginTurnEvent = std::make_unique<EvtDataBeginTurn>(pimpl->m_CurrentTurnIndex, pimpl->m_CurrentPlayerID);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(beginTurnEvent));
}

bool TurnManagerScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void TurnManagerScript::vPostInit()
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataTurnEnded::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onTurnEnded(static_cast<const EvtDataTurnEnded &>(e));
	});
	eventDispatcher->vAddListener(EvtDataBeginTurnEffectDisappeared::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onBeginTurnEffectDisappeared(static_cast<const EvtDataBeginTurnEffectDisappeared &>(e));
	});
	eventDispatcher->vAddListener(EvtDataAllUnitsUnfocused::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onAllUnitsUnfocused(static_cast<const EvtDataAllUnitsUnfocused &>(e));
	});
}

const std::string TurnManagerScript::Type{ "TurnManagerScript" };

const std::string & TurnManagerScript::getType() const
{
	return Type;
}
