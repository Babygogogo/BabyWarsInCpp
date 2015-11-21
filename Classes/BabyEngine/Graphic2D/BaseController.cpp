#include <cassert>

#include "BaseController.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseController.
//////////////////////////////////////////////////////////////////////////
void BaseController::setTarget(const std::shared_ptr<Actor> & actor)
{
	assert(actor && "BaseController::setTarget() the actor is nullptr.");
	assert(actor->getRenderComponent() && "BaseController::setTarget() the actor has no render component.");

	m_TargetActor = actor;

	vSetTargetDelegate(actor);
}

void BaseController::unsetTarget()
{
	setEnable(false);

	m_TargetActor.reset();

	vUnsetTargetDelegate();
}

void BaseController::setEnable(bool enable)
{
	assert(!m_TargetActor.expired() && "BaseController::setEnable() the target actor is expired (or not set).");

	auto renderComponent = m_TargetActor.lock()->getRenderComponent();
	assert(renderComponent && "BaseController::setEnable() the target actor has no render component.");

	vSetEnableDelegate(enable, renderComponent->getSceneNode());
}

std::shared_ptr<Actor> BaseController::getTarget() const
{
	if (m_TargetActor.expired())
		return nullptr;

	return m_TargetActor.lock();
}
