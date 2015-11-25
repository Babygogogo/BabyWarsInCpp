#include "cocos2d.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/BaseRenderComponent.h"
#include "../../BabyEngine/Actor/TransformComponent.h"
#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "ActionMenuScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionMenuScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionMenuScript::ActionMenuScriptImpl
{
	ActionMenuScriptImpl() = default;
	~ActionMenuScriptImpl() = default;

	static std::string s_BackgroundActorPath;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<TransformComponent> m_TransformComponent;
};

std::string ActionMenuScript::ActionMenuScriptImpl::s_BackgroundActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionMenuScript.
//////////////////////////////////////////////////////////////////////////
ActionMenuScript::ActionMenuScript() : pimpl{ std::make_unique<ActionMenuScriptImpl>() }
{
}

ActionMenuScript::~ActionMenuScript()
{
}

bool ActionMenuScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMembersInitialized = false;
	if (isStaticMembersInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	ActionMenuScriptImpl::s_BackgroundActorPath = relatedActorsElement->Attribute("Background");

	return true;
}

void ActionMenuScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();

	auto renderComponent = ownerActor->getRenderComponent();
	assert(renderComponent && "ActionMenuScript::vPostInit() the actor has no render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	auto transformComponent = ownerActor->getComponent<TransformComponent>();
	assert(transformComponent && "ActionMenuScript::vPostInit() the actor has no transform component.");
	pimpl->m_TransformComponent = std::move(transformComponent);

	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	auto backgroundActor = gameLogic->createActor(ActionMenuScriptImpl::s_BackgroundActorPath.c_str());
	backgroundActor->getComponent<TransformComponent>()->setContentSize({ 100, 100 });
	ownerActor->addChild(*backgroundActor);
}

const std::string ActionMenuScript::Type{ "ActionMenuScript" };

const std::string & ActionMenuScript::getType() const
{
	return Type;
}
