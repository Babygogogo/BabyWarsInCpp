#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/XMLToSize.h"
#include "ActionListScript.h"
#include "ActionMenuPanelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionMenuPanelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionMenuPanelScript::ActionMenuPanelScriptImpl
{
	ActionMenuPanelScriptImpl() = default;
	~ActionMenuPanelScriptImpl() = default;

	static std::string s_BackgroundActorPath;
	static std::string s_ListActorPath;

	std::weak_ptr<ActionListScript> m_ListScript;
};

std::string ActionMenuPanelScript::ActionMenuPanelScriptImpl::s_BackgroundActorPath;
std::string ActionMenuPanelScript::ActionMenuPanelScriptImpl::s_ListActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionMenuPanelScript.
//////////////////////////////////////////////////////////////////////////
ActionMenuPanelScript::ActionMenuPanelScript() : pimpl{ std::make_unique<ActionMenuPanelScriptImpl>() }
{
}

ActionMenuPanelScript::~ActionMenuPanelScript()
{
}

bool ActionMenuPanelScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMembersInitialized = false;
	if (isStaticMembersInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	ActionMenuPanelScriptImpl::s_BackgroundActorPath = relatedActorsElement->Attribute("Background");
	ActionMenuPanelScriptImpl::s_ListActorPath = relatedActorsElement->Attribute("ActionList");

	isStaticMembersInitialized = true;
	return true;
}

void ActionMenuPanelScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto sceneNode = ownerActor->getRenderComponent()->getSceneNode();

	auto backgroundActor = gameLogic->createActor(ActionMenuPanelScriptImpl::s_BackgroundActorPath.c_str());
	ownerActor->addChild(*backgroundActor);
	sceneNode->addChild(backgroundActor->getRenderComponent()->getSceneNode());

	auto listActor = gameLogic->createActor(ActionMenuPanelScriptImpl::s_ListActorPath.c_str());
	ownerActor->addChild(*listActor);
	sceneNode->addChild(listActor->getRenderComponent()->getSceneNode());
	pimpl->m_ListScript = listActor->getComponent<ActionListScript>();
}

const std::string ActionMenuPanelScript::Type{ "ActionMenuPanelScript" };

const std::string & ActionMenuPanelScript::getType() const
{
	return Type;
}
