#include "ActorComponent.h"

ActorComponent::~ActorComponent()
{
}

void ActorComponent::setOwner(const std::shared_ptr<Actor> & owner)
{
	m_OwnerActor = owner;
}

bool ActorComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void ActorComponent::vPostInit()
{
}

void ActorComponent::vOnChanged()
{
}
