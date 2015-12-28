#include <cassert>
#include <algorithm>

#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "PlayerScript.h"
#include "PlayerManagerScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of PlayerManagerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct PlayerManagerScript::PlayerManagerScriptImpl
{
	PlayerManagerScriptImpl() = default;
	~PlayerManagerScriptImpl() = default;

	auto getPlayerIterWithID(PlayerID id) const;

	void removeExpiredPlayersFromList();

	static std::string s_PlayerActorPath;

	std::vector<std::weak_ptr<PlayerScript>> m_Players;
};

auto PlayerManagerScript::PlayerManagerScriptImpl::getPlayerIterWithID(PlayerID id) const
{
	return std::find_if(m_Players.begin(), m_Players.end(), [id](const auto & weakPlayer) {
		return weakPlayer.lock()->getID() == id;
	});
}

void PlayerManagerScript::PlayerManagerScriptImpl::removeExpiredPlayersFromList()
{
	std::remove_if(m_Players.begin(), m_Players.end(), [](const auto & weakPlayer) {
		return weakPlayer.expired();
	});
}

std::string PlayerManagerScript::PlayerManagerScriptImpl::s_PlayerActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of PlayerManagerScript.
//////////////////////////////////////////////////////////////////////////
PlayerManagerScript::PlayerManagerScript() : pimpl{ std::make_unique<PlayerManagerScriptImpl>() }
{
}

PlayerManagerScript::~PlayerManagerScript()
{
}

void PlayerManagerScript::loadPlayers(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "PlayerManagerScript::loadPlayers() the xml element is nullptr.");

	pimpl->m_Players.clear();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	for (auto playerElement = xmlElement->FirstChildElement("Player"); playerElement; playerElement = playerElement->NextSiblingElement("Player")) {
		auto playerActor = gameLogic->createActorAndChildren(PlayerManagerScriptImpl::s_PlayerActorPath.c_str());

		auto playerScript = playerActor->getComponent<PlayerScript>();
		playerScript->loadPlayer(playerElement);
		pimpl->m_Players.emplace_back(std::move(playerScript));
	}
}

bool PlayerManagerScript::hasPlayerID(PlayerID id) const
{
	return pimpl->getPlayerIterWithID(id) != pimpl->m_Players.end();
}

bool PlayerManagerScript::hasPlayerNextOfID(PlayerID id) const
{
	auto playerIter = pimpl->getPlayerIterWithID(id);
	if (playerIter == pimpl->m_Players.end()) {
		return false;
	}

	return ++playerIter != pimpl->m_Players.end();
}

PlayerID PlayerManagerScript::getFirstPlayerID() const
{
	pimpl->removeExpiredPlayersFromList();
	assert(!pimpl->m_Players.empty() && "PlayerManagerScript::getFirstPlayerID() there are no players attached to the manager.");

	return pimpl->m_Players[0].lock()->getID();
}

PlayerID PlayerManagerScript::getNextPlayerID(PlayerID currentPlayerID) const
{
	auto playerIter = pimpl->getPlayerIterWithID(currentPlayerID);
	if (playerIter == pimpl->m_Players.end()) {
		return INVALID_PLAYER_ID;
	}

	if (++playerIter == pimpl->m_Players.end()) {
		return INVALID_PLAYER_ID;
	}
	else {
		return playerIter->lock()->getID();
	}
}

bool PlayerManagerScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized) {
		return true;
	}

	auto relatedActorElement = xmlElement->FirstChildElement("RelatedActorsPath");
	PlayerManagerScriptImpl::s_PlayerActorPath = relatedActorElement->Attribute("Player");

	isStaticMemberInitialized = true;
	return true;
}

void PlayerManagerScript::vPostInit()
{
}

const std::string PlayerManagerScript::Type{ "PlayerManagerScript" };

const std::string & PlayerManagerScript::getType() const
{
	return Type;
}
