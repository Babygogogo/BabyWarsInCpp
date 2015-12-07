#include <cassert>

#include "cocos2d.h"

#include "BaseHumanView.h"
#include "../Actor/Actor.h"
#include "../Actor/SceneRenderComponent.h"
#include "../Graphic2D/BaseController.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BaseHumanViewImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseHumanView::BaseHumanViewImpl
{
	BaseHumanViewImpl() = default;
	~BaseHumanViewImpl() = default;

	bool canSetAndRunSceneActor(const std::shared_ptr<Actor> & actor) const;

	std::weak_ptr<BaseHumanView> m_Self;

	std::weak_ptr<Actor> m_SceneActor;
	std::unique_ptr<BaseController> m_Controller;
};

bool BaseHumanView::BaseHumanViewImpl::canSetAndRunSceneActor(const std::shared_ptr<Actor> & actor) const
{
	assert(!m_Self.expired() && "BaseHumanView::canSetAndRunSceneActor() the view is not initialized.");
	assert(m_Self.lock()->isAttachedToLogic() && "BaseHumanView::canSetAndRunSceneActor() the view is not attached to game logic.");

	assert(actor && "BaseHumanView::canSetAndRunSceneActor() the actor is nullptr.");
	assert(actor->getRenderComponent<SceneRenderComponent>() && "BaseHumanView::canSetAndRunSceneActor() the actor has no scene render component.");

	return true;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseHumanView.
//////////////////////////////////////////////////////////////////////////
BaseHumanView::BaseHumanView() : pimpl{ std::make_shared<BaseHumanViewImpl>() }
{
}

BaseHumanView::~BaseHumanView()
{
}

void BaseHumanView::init(std::weak_ptr<BaseHumanView> self)
{
	pimpl->m_Self = self;

	vLoadResource();
}

bool BaseHumanView::setAndRunSceneActor(const std::shared_ptr<Actor> & actor)
{
	if (!pimpl->canSetAndRunSceneActor(actor))
		return false;

	auto sceneRenderComponent = actor->getRenderComponent<SceneRenderComponent>();
	auto sceneToRun = sceneRenderComponent->getTransitionScene() ? sceneRenderComponent->getTransitionScene() : sceneRenderComponent->getScene();

	if (pimpl->m_SceneActor.expired()) {
		cocos2d::Director::getInstance()->runWithScene(sceneToRun);
	}
	else {
		cocos2d::Director::getInstance()->replaceScene(sceneToRun);
	}

	pimpl->m_SceneActor = actor;

	return true;
}

std::shared_ptr<Actor> BaseHumanView::getSceneActor() const
{
	if (pimpl->m_SceneActor.expired())
		return nullptr;

	return pimpl->m_SceneActor.lock();
}

void BaseHumanView::setController(std::unique_ptr<BaseController> controller)
{
	pimpl->m_Controller = std::move(controller);
}
