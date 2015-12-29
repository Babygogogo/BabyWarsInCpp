#include <vector>

#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataAllUnitsUnfocused.h"
#include "../Event/EvtDataGameCommandGenerated.h"
#include "../Event/EvtDataRequestChangeTurnPhase.h"
#include "../Event/EvtDataTurnPhaseChanged.h"
#include "../Utilities/GameCommand.h"
#include "../Utilities/TurnPhase.h"
#include "../Utilities/TurnPhaseFactory.h"
#include "../Utilities/TurnPhaseTypeCode.h"
#include "../Utilities/XMLToPlayerID.h"
#include "PlayerManagerScript.h"
#include "TurnManagerScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TurnManagerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TurnManagerScript::TurnManagerScriptImpl
{
	TurnManagerScriptImpl() = default;
	~TurnManagerScriptImpl() = default;

	void onRequestChangeTurnPhase(const EvtDataRequestChangeTurnPhase & e);
	void onAllUnitsUnfocused(const EvtDataAllUnitsUnfocused & e);

	void setPhaseAndQueueEvent(TurnPhaseTypeCode typeCode);
	void queueEventTurnPhaseChanged() const;

	bool isTurnIndexValid() const;
	bool isPlayerIdValid() const;
	bool isTurnPhaseValid() const;

	bool updateTurnIndexAndPlayerIdForNewTurn();
	TurnIndex getNextTurnIndex(TurnIndex currentTurnIndex) const;

	void setPhase(std::shared_ptr<TurnPhase> && turnPhase);

	std::vector<GameCommand> generateGameCommands() const;

	TurnIndex m_CurrentTurnIndex{ INVALID_TURN_INDEX };
	PlayerID m_CurrentPlayerID{ INVALID_PLAYER_ID };

	std::shared_ptr<TurnPhase> m_TurnPhase{ utilities::createTurnPhaseWithTypeCode(TurnPhaseTypeCode::Invalid) };

	std::weak_ptr<TurnManagerScript> m_SelfScript;
	std::weak_ptr<PlayerManagerScript> m_PlayerManagerScript;
};

void TurnManagerScript::TurnManagerScriptImpl::onRequestChangeTurnPhase(const EvtDataRequestChangeTurnPhase & e)
{
	assert(m_TurnPhase && "TurnManagerScriptImpl::onRequestChangeTurnPhase() the current turn phase is nullptr.");
	const auto newPhaseTypeCode = e.getTurnPhaseTypeCode();
	if (m_TurnPhase->vCanEnterNewPhase(newPhaseTypeCode)) {
		setPhaseAndQueueEvent(newPhaseTypeCode);

		auto eventGameCommandGenerated = std::make_unique<EvtDataGameCommandGenerated>(m_TurnPhase->vGenerateGameCommands());
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(eventGameCommandGenerated));
	}
}

void TurnManagerScript::TurnManagerScriptImpl::onAllUnitsUnfocused(const EvtDataAllUnitsUnfocused & e)
{
	auto commandGeneratedEvent = std::make_unique<EvtDataGameCommandGenerated>(generateGameCommands());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(commandGeneratedEvent));
}

void TurnManagerScript::TurnManagerScriptImpl::setPhaseAndQueueEvent(TurnPhaseTypeCode typeCode)
{
	if (m_TurnPhase->vGetTypeCode() != typeCode) {
		setPhase(utilities::createTurnPhaseWithTypeCode(typeCode));
		queueEventTurnPhaseChanged();
	}
}

void TurnManagerScript::TurnManagerScriptImpl::queueEventTurnPhaseChanged() const
{
	//The EvtDataTurnPhaseChanged should be triggered ASAP instead of being queued, because a phase may immediately change to a new phase before the event in queue being dispatched.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vTrigger(std::make_unique<EvtDataTurnPhaseChanged>(m_SelfScript, m_TurnPhase));
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

bool TurnManagerScript::TurnManagerScriptImpl::isTurnPhaseValid() const
{
	return m_TurnPhase && m_TurnPhase->vGetTypeCode() != TurnPhaseTypeCode::Invalid;
}

bool TurnManagerScript::TurnManagerScriptImpl::updateTurnIndexAndPlayerIdForNewTurn()
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::updateTurnIndexAndPlayerIdForNewTurn() the player manager script is expired.");
	auto playerManager = m_PlayerManagerScript.lock();

	if (playerManager->hasPlayerNextOfID(m_CurrentPlayerID)) {
		m_CurrentPlayerID = playerManager->getNextPlayerID(m_CurrentPlayerID);
	}
	else {
		m_CurrentPlayerID = playerManager->getFirstPlayerID();
		m_CurrentTurnIndex = getNextTurnIndex(m_CurrentTurnIndex);
	}

	//#TODO: Return false if can't update the values.
	return true;
}

TurnIndex TurnManagerScript::TurnManagerScriptImpl::getNextTurnIndex(TurnIndex currentTurnIndex) const
{
	return currentTurnIndex + 1;
}

void TurnManagerScript::TurnManagerScriptImpl::setPhase(std::shared_ptr<TurnPhase> && turnPhase)
{
	assert(turnPhase && "TurnManagerScriptImpl::setPhase() the phase is nullptr.");
	m_TurnPhase = turnPhase;
	m_TurnPhase->vOnEnterPhase(*m_SelfScript.lock());
}

std::vector<GameCommand> TurnManagerScript::TurnManagerScriptImpl::generateGameCommands() const
{
	auto commands = std::vector<GameCommand>{};

	commands.emplace_back("End Turn", [turnIndex = m_CurrentTurnIndex, playerID = m_CurrentPlayerID]() {
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestChangeTurnPhase>(TurnPhaseTypeCode::End));
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
	pimpl->m_CurrentPlayerID = utilities::XMLToPlayerID(currentTurnElement->FirstChildElement("PlayerID"));
	pimpl->m_TurnPhase = utilities::createTurnPhaseWithXML(currentTurnElement);
}

void TurnManagerScript::run()
{
	assert(pimpl->isTurnIndexValid() && "TurnManagerScript::run() the turn index is invalid.");
	assert(pimpl->isPlayerIdValid() && "TurnManagerScript::run() the player id is invalid.");
	assert(pimpl->isTurnPhaseValid() && "TurnManagerScript::run() the turn phase is invalid.");

	pimpl->queueEventTurnPhaseChanged();
}

void TurnManagerScript::updateTurnIndexAndPlayerIdForNewTurn()
{
	pimpl->updateTurnIndexAndPlayerIdForNewTurn();
}

PlayerID TurnManagerScript::getCurrentPlayerID() const
{
	return pimpl->m_CurrentPlayerID;
}

TurnIndex TurnManagerScript::getCurrentTurnIndex() const
{
	return pimpl->m_CurrentTurnIndex;
}

TurnPhaseTypeCode TurnManagerScript::getCurrentPhaseTypeCode() const
{
	assert(pimpl->m_TurnPhase && "TurnManagerScript::getCurrentPhaseTypeCode() the turn phase is nullptr.");
	return pimpl->m_TurnPhase->vGetTypeCode();
}

//void TurnManagerScript::setPhaseAndQueueEvent(TurnPhaseTypeCode typeCode)
//{
//	pimpl->setPhaseAndQueueEvent(typeCode);
//}

bool TurnManagerScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void TurnManagerScript::vPostInit()
{
	pimpl->m_SelfScript = getComponent<TurnManagerScript>();

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EvtDataRequestChangeTurnPhase::s_EventType, pimpl, [this](const auto & e) {
		pimpl->onRequestChangeTurnPhase(static_cast<const EvtDataRequestChangeTurnPhase &>(e));
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
