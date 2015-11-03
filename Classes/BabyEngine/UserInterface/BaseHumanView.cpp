#include <cassert>
#include <unordered_map>

#include "cocos2d.h"

#include "BaseHumanView.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BaseHumanViewImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseHumanView::BaseHumanViewImpl
{
	BaseHumanViewImpl();
	~BaseHumanViewImpl();

	void onRequestDestroyActor(const IEventData & e);

	void removeActor(ActorID actorID);

	std::weak_ptr<BaseHumanView> m_Self;
	std::unordered_map<ActorID, std::weak_ptr<Actor>> m_Actors;
	cocos2d::Node * m_Node{ cocos2d::Node::create() };
};

BaseHumanView::BaseHumanViewImpl::BaseHumanViewImpl()
{
	m_Node->retain();
}

BaseHumanView::BaseHumanViewImpl::~BaseHumanViewImpl()
{
	m_Node->removeFromParent();
	m_Node->removeAllChildren();
	m_Node->release();
}

void BaseHumanView::BaseHumanViewImpl::onRequestDestroyActor(const IEventData & e)
{
	const auto & event = static_cast<const EvtDataRequestDestroyActor &>(e);
	removeActor(event.getActorID());
}

void BaseHumanView::BaseHumanViewImpl::removeActor(ActorID actorID)
{
	const auto actorIter = m_Actors.find(actorID);
	if (actorIter == m_Actors.end())
		return;

	if (!actorIter->second.expired()){
		auto actor = actorIter->second.lock();
		if (auto renderComponent = actor->getRenderComponent())
			m_Node->removeChild(renderComponent->getSceneNode());
		actor->setHumanView(std::weak_ptr<BaseHumanView>());
	}

	m_Actors.erase(actorIter);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseHumanView.
//////////////////////////////////////////////////////////////////////////
BaseHumanView::BaseHumanView() : pimpl{ std::make_shared<BaseHumanViewImpl>() }
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EvtDataRequestDestroyActor::s_EventType, pimpl, [this](const IEventData & e){pimpl->onRequestDestroyActor(e); });
}

BaseHumanView::~BaseHumanView()
{
}

void BaseHumanView::init(std::weak_ptr<BaseHumanView> self)
{
	pimpl->m_Self = self;

	vLoadResource();
}

void BaseHumanView::addActor(const std::shared_ptr<Actor> & actor)
{
	if (!actor)
		return;

	assert(!actor->hasParent() && "BaseHumanView::addActor() the actor has parent already.");
	assert(!actor->isAttachedToHumanView() && "BaseHumanView::addActor() the actor has been attached to view already.");
	assert(!pimpl->m_Self.expired() && "BaseHumanView::addActor() the view is not initialized.");

	if (auto renderComponent = actor->getRenderComponent())
		pimpl->m_Node->addChild(renderComponent->getSceneNode());

	pimpl->m_Actors.emplace(actor->getID(), actor);
	actor->setHumanView(pimpl->m_Self);
}

void BaseHumanView::removeActor(ActorID actorID)
{
	pimpl->removeActor(actorID);
}

cocos2d::Node * BaseHumanView::getSceneNode() const
{
	return pimpl->m_Node;
}
