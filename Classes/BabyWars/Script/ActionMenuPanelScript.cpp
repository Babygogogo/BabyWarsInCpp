#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/XMLToSize.h"
#include "ActionMenuPanelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionMenuPanelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionMenuPanelScript::ActionMenuPanelScriptImpl
{
	ActionMenuPanelScriptImpl() = default;
	~ActionMenuPanelScriptImpl() = default;

	static std::string s_BackgroundActorPath;
	static cocos2d::Size s_PanelSize;
};

std::string ActionMenuPanelScript::ActionMenuPanelScriptImpl::s_BackgroundActorPath;
cocos2d::Size ActionMenuPanelScript::ActionMenuPanelScriptImpl::s_PanelSize;

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

	ActionMenuPanelScriptImpl::s_PanelSize = utilities::XMLToSize(xmlElement->FirstChildElement("PanelSize"));

	isStaticMembersInitialized = true;
	return true;
}

void ActionMenuPanelScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();

	auto backgroundActor = gameLogic->createActor(ActionMenuPanelScriptImpl::s_BackgroundActorPath.c_str());
	backgroundActor->getComponent<TransformComponent>()->setContentSize(ActionMenuPanelScriptImpl::s_PanelSize);
	ownerActor->addChild(*backgroundActor);
}

const std::string ActionMenuPanelScript::Type{ "ActionMenuPanelScript" };

const std::string & ActionMenuPanelScript::getType() const
{
	return Type;
}
