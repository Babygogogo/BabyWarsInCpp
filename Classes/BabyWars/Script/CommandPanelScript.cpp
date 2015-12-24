#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "../../BabyEngine/Utilities/XMLToSize.h"
#include "CommandListScript.h"
#include "CommandPanelScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CommandPanelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CommandPanelScript::CommandPanelScriptImpl
{
	CommandPanelScriptImpl() = default;
	~CommandPanelScriptImpl() = default;

	static std::string s_BackgroundActorPath;
	static std::string s_ListActorPath;

	std::weak_ptr<CommandListScript> m_ListScript;
};

std::string CommandPanelScript::CommandPanelScriptImpl::s_BackgroundActorPath;
std::string CommandPanelScript::CommandPanelScriptImpl::s_ListActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of CommandPanelScript.
//////////////////////////////////////////////////////////////////////////
CommandPanelScript::CommandPanelScript() : pimpl{ std::make_unique<CommandPanelScriptImpl>() }
{
}

CommandPanelScript::~CommandPanelScript()
{
}

bool CommandPanelScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMembersInitialized = false;
	if (isStaticMembersInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	CommandPanelScriptImpl::s_BackgroundActorPath = relatedActorsElement->Attribute("Background");
	CommandPanelScriptImpl::s_ListActorPath = relatedActorsElement->Attribute("CommandList");

	isStaticMembersInitialized = true;
	return true;
}

void CommandPanelScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto sceneNode = ownerActor->getRenderComponent()->getSceneNode();

	auto backgroundActor = gameLogic->createActorAndChildren(CommandPanelScriptImpl::s_BackgroundActorPath.c_str());
	ownerActor->addChild(*backgroundActor);
	//sceneNode->addChild(backgroundActor->getRenderComponent()->getSceneNode());

	auto listActor = gameLogic->createActorAndChildren(CommandPanelScriptImpl::s_ListActorPath.c_str());
	ownerActor->addChild(*listActor);
	//sceneNode->addChild(listActor->getRenderComponent()->getSceneNode());
	pimpl->m_ListScript = listActor->getComponent<CommandListScript>();
}

const std::string CommandPanelScript::Type{ "CommandPanelScript" };

const std::string & CommandPanelScript::getType() const
{
	return Type;
}
