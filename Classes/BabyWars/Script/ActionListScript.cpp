#include "cocos2d/external/tinyxml2/tinyxml2.h"

#include "../../BabyEngine/GameLogic/BaseGameLogic.h"
#include "../../BabyEngine/Actor/Actor.h"
#include "../../BabyEngine/Actor/ListViewRenderComponent.h"
#include "../../BabyEngine/Utilities/SingletonContainer.h"
#include "ActionListScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionListScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionListScript::ActionListScriptImpl
{
	ActionListScriptImpl() = default;
	~ActionListScriptImpl() = default;

	static std::string s_ListItemActorPath;

	std::weak_ptr<ListViewRenderComponent> m_RenderComponent;
};

std::string ActionListScript::ActionListScriptImpl::s_ListItemActorPath;

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionListScript.
//////////////////////////////////////////////////////////////////////////
ActionListScript::ActionListScript() : pimpl{ std::make_unique<ActionListScriptImpl>() }
{
}

ActionListScript::~ActionListScript()
{
}

bool ActionListScript::vInit(const tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticMemberInitialized = false;
	if (isStaticMemberInitialized)
		return true;

	const auto relatedActorsElement = xmlElement->FirstChildElement("RelatedActorsPath");
	ActionListScriptImpl::s_ListItemActorPath = relatedActorsElement->Attribute("ListItem");

	isStaticMemberInitialized = true;
	return true;
}

void ActionListScript::vPostInit()
{
	auto ownerActor = m_OwnerActor.lock();

	auto renderComponent = ownerActor->getRenderComponent<ListViewRenderComponent>();
	assert(renderComponent && "ActionListScript::vPostInit() the actor has no list view render component.");
	pimpl->m_RenderComponent = std::move(renderComponent);

	//#TODO: This is for test only. Should be removed.
	auto gameLogic = SingletonContainer::getInstance()->get<BaseGameLogic>();
	for (auto i = 0; i < 10; ++i) {
		ownerActor->addChild(*gameLogic->createActor(ActionListScriptImpl::s_ListItemActorPath.c_str()));
	}
}

const std::string ActionListScript::Type{ "ActionListScript" };

const std::string & ActionListScript::getType() const
{
	return Type;
}
