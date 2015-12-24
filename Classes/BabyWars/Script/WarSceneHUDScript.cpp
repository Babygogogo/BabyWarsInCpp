#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "BeginTurnEffectScript.h"
#include "MoneyInfoPanelScript.h"
#include "TerrainInfoPanelScript.h"
#include "CommandPanelScript.h"
#include "WarSceneHUDScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of WarSceneHUDScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct WarSceneHUDScript::WarSceneHUDScriptImpl
{
	WarSceneHUDScriptImpl() = default;
	~WarSceneHUDScriptImpl() = default;

	static std::string s_BeginTurnEffectActorPath;
	static std::string s_MoneyInfoPanelActorPath;
	static std::string s_CommandPanelActorPath;
	static std::string s_TerrainInfoPanelActorPath;

	std::weak_ptr<BeginTurnEffectScript> m_BeginTurnEffectScript;
	std::weak_ptr<MoneyInfoPanelScript> m_MoneyInfoPanelScript;
	std::weak_ptr<CommandPanelScript> m_CommandPanelScript;
	std::weak_ptr<TerrainInfoPanelScript> m_TerrainInfoPanelScript;
};

std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_BeginTurnEffectActorPath;
std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath;
std::string WarSceneHUDScript::WarSceneHUDScriptImpl::s_CommandPanelActorPath;
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

bool WarSceneHUDScript::onInputTouch(const EvtDataInputTouch & touch)
{
	if (pimpl->m_BeginTurnEffectScript.lock()->onInputTouch(touch)) {
		return true;
	}

	return false;
}

bool WarSceneHUDScript::vInit(const tinyxml2::XMLElement * xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	WarSceneHUDScriptImpl::s_BeginTurnEffectActorPath = relatedActorsElement->Attribute("BeginTurnEffect");
	WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath = relatedActorsElement->Attribute("MoneyInfoPanel");
	WarSceneHUDScriptImpl::s_CommandPanelActorPath = relatedActorsElement->Attribute("CommandPanel");
	WarSceneHUDScriptImpl::s_TerrainInfoPanelActorPath = relatedActorsElement->Attribute("TerrainInfoPanel");

	isStaticMemberInitialized = true;
	return true;
}

void WarSceneHUDScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto selfSceneNode = ownerActor->getRenderComponent()->getSceneNode();

	auto beginTurnEffectActor = gameLogic->createActorAndChildren(WarSceneHUDScriptImpl::s_BeginTurnEffectActorPath.c_str());
	pimpl->m_BeginTurnEffectScript = beginTurnEffectActor->getComponent<BeginTurnEffectScript>();
	ownerActor->addChild(*beginTurnEffectActor);
	//selfSceneNode->addChild(beginTurnEffectActor->getRenderComponent()->getSceneNode());

	auto moneyInfoPanelActor = gameLogic->createActorAndChildren(WarSceneHUDScriptImpl::s_MoneyInfoPanelActorPath.c_str());
	pimpl->m_MoneyInfoPanelScript = moneyInfoPanelActor->getComponent<MoneyInfoPanelScript>();
	ownerActor->addChild(*moneyInfoPanelActor);
	//selfSceneNode->addChild(moneyInfoPanelActor->getRenderComponent()->getSceneNode());

	auto actionMenuPanelActor = gameLogic->createActorAndChildren(WarSceneHUDScriptImpl::s_CommandPanelActorPath.c_str());
	pimpl->m_CommandPanelScript = actionMenuPanelActor->getComponent<CommandPanelScript>();
	ownerActor->addChild(*actionMenuPanelActor);
	//selfSceneNode->addChild(actionMenuPanelActor->getRenderComponent()->getSceneNode());

	auto terrainInfoPanelActor = gameLogic->createActorAndChildren(WarSceneHUDScriptImpl::s_TerrainInfoPanelActorPath.c_str());
	pimpl->m_TerrainInfoPanelScript = terrainInfoPanelActor->getComponent<TerrainInfoPanelScript>();
	ownerActor->addChild(*terrainInfoPanelActor);
	//selfSceneNode->addChild(terrainInfoPanelActor->getRenderComponent()->getSceneNode());
}

const std::string WarSceneHUDScript::Type{ "WarSceneHUDScript" };

const std::string & WarSceneHUDScript::getType() const
{
	return Type;
}
