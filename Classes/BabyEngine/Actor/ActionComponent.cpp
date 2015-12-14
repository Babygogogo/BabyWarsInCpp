#include <cassert>

#include "cocos2d.h"

#include "ActionComponent.h"
#include "BaseRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActionComponentImpl.
//////////////////////////////////////////////////////////////////////////
struct ActionComponent::ActionComponentImpl
{
	ActionComponentImpl() = default;
	~ActionComponentImpl() = default;

	std::shared_ptr<BaseRenderComponent> m_RenderComponent;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of ActionComponent.
//////////////////////////////////////////////////////////////////////////
ActionComponent::ActionComponent() : pimpl{ std::make_unique<ActionComponentImpl>() }
{
}

ActionComponent::~ActionComponent()
{
}

void ActionComponent::runAction(cocos2d::Action * action)
{
	pimpl->m_RenderComponent->getSceneNode()->runAction(action);
}

void ActionComponent::stopAction(cocos2d::Action * action)
{
	pimpl->m_RenderComponent->getSceneNode()->stopAction(action);
}

void ActionComponent::stopAllActions()
{
	pimpl->m_RenderComponent->getSceneNode()->stopAllActions();
}

void ActionComponent::vPostInit()
{
	pimpl->m_RenderComponent = getRenderComponent();
	assert(pimpl->m_RenderComponent && "ActionComponent::vPostInit() the owner actor has no render component.");
}

const std::string ActionComponent::Type{ "ActionComponent" };

const std::string & ActionComponent::getType() const
{
	return Type;
}
