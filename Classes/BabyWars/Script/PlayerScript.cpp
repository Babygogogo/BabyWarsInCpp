#include "../cocos2d/external/tinyxml2/tinyxml2.h"

#include "PlayerScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of PlayerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct PlayerScript::PlayerScriptImpl
{
	PlayerScriptImpl() = default;
	~PlayerScriptImpl() = default;

	PlayerID m_ID{ INVALID_PLAYER_ID };
	std::string m_Name;
	int m_Money{};
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

	pimpl->m_ID = xmlElement->IntAttribute("ID");
	pimpl->m_Name = xmlElement->Attribute("Name");
	pimpl->m_Money = xmlElement->IntAttribute("Money");
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
