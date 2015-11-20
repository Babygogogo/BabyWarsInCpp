#include <cassert>

#include "BaseController.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BaseControllerImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseController::BaseControllerImpl
{
	BaseControllerImpl() = default;
	~BaseControllerImpl() = default;

	std::weak_ptr<Actor> m_TargetActor;
};

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseController.
//////////////////////////////////////////////////////////////////////////
BaseController::BaseController() : pimpl{ std::make_unique<BaseControllerImpl>() }
{
}

BaseController::~BaseController()
{
}

void BaseController::setTarget(const std::shared_ptr<Actor> & actor)
{
	assert(actor && "BaseController::setTarget() the actor is nullptr.");
	assert(actor->getRenderComponent() && "BaseController::setTarget() the actor has no render component.");

	pimpl->m_TargetActor = actor;

	vSetTargetDelegate(actor);
}

void BaseController::unsetTarget()
{
	setEnable(false);

	pimpl->m_TargetActor.reset();

	vUnsetTargetDelegate();
}

void BaseController::setEnable(bool enable)
{
	assert(!pimpl->m_TargetActor.expired() && "BaseController::setEnable() the target actor is expired (or not set).");

	auto renderComponent = pimpl->m_TargetActor.lock()->getRenderComponent();
	assert(renderComponent && "BaseController::setEnable() the target actor has no render component.");

	vSetEnableDelegate(enable, renderComponent->getSceneNode());
}
