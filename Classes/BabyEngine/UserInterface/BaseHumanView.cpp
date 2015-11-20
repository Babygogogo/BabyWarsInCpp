#include <cassert>

#include "cocos2d.h"

#include "BaseHumanView.h"
#include "../Actor/Actor.h"
#include "../Actor/SceneRenderComponent.h"
#include "../Graphic2D/IController.h"

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
	std::unique_ptr<IController> m_Controller;
};

bool BaseHumanView::BaseHumanViewImpl::canSetAndRunSceneActor(const std::shared_ptr<Actor> & actor) const
{
	assert(!m_Self.expired() && "BaseHumanView::canSetAndRunSceneActor() the view is not initialized.");
	assert(m_Self.lock()->isAttachedToLogic() && "BaseHumanView::canSetAndRunSceneActor() the view is not attached to game logic.");

	assert(actor && "BaseHumanView::canSetAndRunSceneActor() the actor is nullptr.");
	assert(!actor->hasParent() && "BaseHumanView::canSetAndRunSceneActor() the actor has parent already.");
	assert(!actor->isAttachedToHumanView() && "BaseHumanView::canSetAndRunSceneActor() the actor has been attached to view already.");

	auto sceneRenderComponent = std::dynamic_pointer_cast<SceneRenderComponent>(actor->getRenderComponent());
	assert(sceneRenderComponent && "BaseHumanView::canSetAndRunSceneActor() the actor has no scene render component.");

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

	auto sceneRenderComponent = std::dynamic_pointer_cast<SceneRenderComponent>(actor->getRenderComponent());
	auto sceneToRun = sceneRenderComponent->getTransitionScene() ? sceneRenderComponent->getTransitionScene() : sceneRenderComponent->getScene();

	if (pimpl->m_SceneActor.expired())
		cocos2d::Director::getInstance()->runWithScene(sceneToRun);
	else {
		pimpl->m_SceneActor.lock()->setHumanView(std::weak_ptr<BaseHumanView>());
		cocos2d::Director::getInstance()->replaceScene(sceneToRun);
	}

	actor->setHumanView(pimpl->m_Self);
	pimpl->m_SceneActor = actor;

	return true;
}

std::shared_ptr<Actor> BaseHumanView::getSceneActor() const
{
	if (pimpl->m_SceneActor.expired())
		return nullptr;

	return pimpl->m_SceneActor.lock();
}

void BaseHumanView::setController(std::unique_ptr<IController> controller)
{
	pimpl->m_Controller = std::move(controller);
}
