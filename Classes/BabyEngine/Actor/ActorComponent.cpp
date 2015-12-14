#include <cassert>

#include "Actor.h"
#include "ActorComponent.h"

std::shared_ptr<ActorComponent> ActorComponent::getComponent(const std::string & type) const
{
	assert(!m_OwnerActor.expired() && "ActorComponent::getComponent() the owner actor is expired.");

	return m_OwnerActor.lock()->getComponent(type);
}

std::shared_ptr<BaseRenderComponent> ActorComponent::getRenderComponent() const
{
	assert(!m_OwnerActor.expired() && "ActorComponent::getComponent() the owner actor is expired.");

	return m_OwnerActor.lock()->getRenderComponent();
}

void ActorComponent::setOwner(const std::shared_ptr<Actor> & owner)
{
	m_OwnerActor = owner;
}

bool ActorComponent::vInit(const tinyxml2::XMLElement * xmlElement)
{
	return true;
}

void ActorComponent::vPostInit()
{
}

void ActorComponent::vOnChanged()
{
}
