#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Event/IEventDispatcher.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../Event/EvtDataBeginTurn.h"
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

	bool isTurnIndexValid() const;
	bool isPlayerIdValid() const;

	void initAsFirstTurnForFirstPlayer();

	int m_CurrentTurnIndex{};
	PlayerID m_CurrentPlayerID{ INVALID_PLAYER_ID };

	std::weak_ptr<PlayerManagerScript> m_PlayerManagerScript;
};

bool TurnManagerScript::TurnManagerScriptImpl::isTurnIndexValid() const
{
	return m_CurrentTurnIndex > 0;
}

bool TurnManagerScript::TurnManagerScriptImpl::isPlayerIdValid() const
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::isPlayerIdValid() the player manager script is expired or not set.");
	return m_PlayerManagerScript.lock()->hasPlayerWithID(m_CurrentPlayerID);
}

void TurnManagerScript::TurnManagerScriptImpl::initAsFirstTurnForFirstPlayer()
{
	assert(!m_PlayerManagerScript.expired() && "TurnManagerScriptImpl::initAsFirstTurnForFirstPlayer() the player manager script is expired or not set.");

	m_CurrentTurnIndex = 1;
	m_CurrentPlayerID = m_PlayerManagerScript.lock()->getFirstPlayerID();
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
}

const std::string TurnManagerScript::Type{ "TurnManagerScript" };

const std::string & TurnManagerScript::getType() const
{
	return Type;
}
