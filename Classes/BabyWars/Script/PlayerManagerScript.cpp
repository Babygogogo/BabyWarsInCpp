#include <cassert>
#include <algorithm>

#include "cocos2d/external/tinyxml2/tinyxml2.h"

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

	void removeExpiredPlayersFromList();

	static std::string s_PlayerActorPath;

	std::vector<std::weak_ptr<PlayerScript>> m_Players;
};

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
		auto playerActor = gameLogic->createActor(PlayerManagerScriptImpl::s_PlayerActorPath.c_str());

		auto playerScript = playerActor->getComponent<PlayerScript>();
		playerScript->loadPlayer(playerElement);
		pimpl->m_Players.emplace_back(std::move(playerScript));
	}
}

bool PlayerManagerScript::hasPlayerWithID(PlayerID id) const
{
	for (const auto weakPlayer : pimpl->m_Players) {
		if (!weakPlayer.expired()) {
			if (weakPlayer.lock()->getID() == id) {
				return true;
			}
		}
	}

	return false;
}

PlayerID PlayerManagerScript::getFirstPlayerID() const
{
	pimpl->removeExpiredPlayersFromList();
	assert(!pimpl->m_Players.empty() && "PlayerManagerScript::getFirstPlayerID() there are no players attached to the manager.");

	return pimpl->m_Players[0].lock()->getID();
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
