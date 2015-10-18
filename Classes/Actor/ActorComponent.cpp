#include <cassert>

#include "ActorComponent.h"

ActorComponent::~ActorComponent()
{
}

void ActorComponent::setOwner(std::weak_ptr<Actor> && owner)
{
	assert(!owner.expired());

	m_OwnerActor = std::move(owner);
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
