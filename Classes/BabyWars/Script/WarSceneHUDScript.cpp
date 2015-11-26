#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "MoneyInfoPanelScript.h"
#include "TerrainInfoPanelScript.h"
#include "ActionMenuPanelScript.h"
#include "WarSceneHUDScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneHUDScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WarSceneHUDScript::WarSceneHUDScriptImpl
{
	WarSceneHUDScriptImpl() = default;
	~WarSceneHUDScriptImpl() = default;

	static std::string s_MoneyInfoPanelActorPath;
	static std::string s_ActionMenuPanelActorPath;
	static std::string s_TerrainInfoPanelActorPath;

	std::weak_ptr<MoneyInfoPanelScript> m_MoneyInfoPanelScript;
	std::weak_ptr<ActionMenuPanelScript> m_ActionMenuPanelScript;
	std::weak_ptr<TerrainInfoPanelScript> m_TerrainInfoPanelScript;
};

std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath;
std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_ActionMenuPanelActorPath;
std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_TerrainInfoPanelActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of WarSceneHUDScript.
//////////////////////////////////////////////////////////////////////////
WarSceneHUDScript::WarSceneHUDScript() : pimpl{ std::make_unique<WarSceneHUDScriptImpl>() }
{
}

WarSceneHUDScript::~WarSceneHUDScript()
{
}

bool WarSceneHUDScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath = relatedActorsElement->Attribute("MoneyInfoPanel");
	WarSceneHUDScriptImpl::s_ActionMenuPanelActorPath = relatedActorsElement->Attribute("ActionMenuPanel");
	WarSceneHUDScriptImpl::s_TerrainInfoPanelActorPath = relatedActorsElement->Attribute("TerrainInfoPanel");

	isStaticMemberInitialized = true;
	return true;
}

void WarSceneHUDScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	auto moneyInfoPanelActor = gameLogic->createActor(WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath.c_str());
	pimpl->m_MoneyInfoPanelScript = moneyInfoPanelActor->getComponent<MoneyInfoPanelScript>();
	ownerActor->addChild(*moneyInfoPanelActor);

	auto actionMenuPanelActor = gameLogic->createActor(WarSceneHUDScriptImpl::s_ActionMenuPanelActorPath.c_str());
	pimpl->m_ActionMenuPanelScript = actionMenuPanelActor->getComponent<ActionMenuPanelScript>();
	ownerActor->addChild(*actionMenuPanelActor);

	auto terrainInfoPanelActor = gameLogic->createActor(WarSceneHUDScriptImpl::s_TerrainInfoPanelActorPath.c_str());
	pimpl->m_TerrainInfoPanelScript = terrainInfoPanelActor->getComponent<TerrainInfoPanelScript>();
	ownerActor->addChild(*terrainInfoPanelActor);
}

const std::string WarSceneHUDScript::Type{ "WarSceneHUDScript" };

const std::string & WarSceneHUDScript::getType() const
{
	return Type;
}
