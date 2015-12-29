#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "../Utilities/XMLToPlayerID.h"
#include "../Utilities/XMLToMoney.h"
#include "PlayerScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of PlayerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct PlayerScript::PlayerScriptImpl
{
	PlayerScriptImpl() = default;
	~PlayerScriptImpl() = default;

	PlayerID m_ID{ INVALID_PLAYER_ID };
	Money m_Money{ INVALID_MONEY };
};

//////////////////////////////////////////////////////////////////////////
//Implementation of PlayerScript.
//////////////////////////////////////////////////////////////////////////
PlayerScript::PlayerScript() : pimpl{ std::make_unique<PlayerScriptImpl>() }
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::loadPlayer(const tinyxml2::XMLElement * xmlElement)
{
	assert(xmlElement && "PlayerScript::loadPlayer() the xml element is nullptr.");

	pimpl->m_ID = utilities::XMLToPlayerID(xmlElement->FirstChildElement("ID"));
	pimpl->m_Money = utilities::XMLToMoney(xmlElement->FirstChildElement("Money"));
}

PlayerID PlayerScript::getID() const
{
	return pimpl->m_ID;
}

bool PlayerScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void PlayerScript::vPostInit()
{
}

const std::string PlayerScript::Type{ "PlayerScript" };

const std::string & PlayerScript::getType() const
{
	return Type;
}
