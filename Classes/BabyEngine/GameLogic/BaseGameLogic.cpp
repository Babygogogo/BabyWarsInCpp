#include <map>
#include <list>
#include <cassert>
#include <mutex>
#include <functional>

#include "cocos2d.h"

#include "BaseGameLogic.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseActorFactory.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../Process/ProcessRunner.h"
#include "../UserInterface/BaseHumanView.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameLogicImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseGameLogic::BaseGameLogicImpl
{
public:
	BaseGameLogicImpl();
	~BaseGameLogicImpl();

	void onRequestDestroyActor(const IEventData & e);

	ProcessRunner m_ProcessRunner;

	//Lock of the process of updating actor.
	bool m_IsUpdatingActors{ false };

	std::weak_ptr<BaseGameLogic> m_Self;

	std::map<ActorID, std::shared_ptr<Actor>> m_Actors;
	std::map<GameViewID, std::shared_ptr<BaseGameView>> m_GameViews;
	std::unique_ptr<BaseActorFactory> m_ActorFactory;

	cocos2d::Scene * m_Scene{ cocos2d::Scene::create() };

	//The operations that are blocked because of the lock. They should be executed when the lock is unlocked.
	std::list<std::function<void()>> m_CachedOperations;
};

BaseGameLogic::BaseGameLogicImpl::BaseGameLogicImpl()
{
	m_Scene->retain();
}

BaseGameLogic::BaseGameLogicImpl::~BaseGameLogicImpl()
{
	//The logic is destroyed only when the game is shutting down.
	//Don't call m_Scene->removeFromParent() because it's done implicitly by the engine and it asserts if we do it manually.
	m_Scene->removeAllChildren();
	m_Scene->release();
}

void BaseGameLogic::BaseGameLogicImpl::onRequestDestroyActor(const IEventData & e)
{
	auto actorID = (static_cast<const EvtDataRequestDestroyActor &>(e)).getActorID();

	if (m_IsUpdatingActors)
		m_CachedOperations.emplace_back([actorID, this](){m_Actors.erase(actorID); });
	else
		m_Actors.erase(actorID);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameLogic.
//////////////////////////////////////////////////////////////////////////
BaseGameLogic::BaseGameLogic() : pimpl{ std::make_shared<BaseGameLogicImpl>() }
{
	static int InstanceCount{ 0 };
	assert((InstanceCount++ == 0) && "GameLogic is created more than once!");
}

BaseGameLogic::~BaseGameLogic()
{
}

void BaseGameLogic::init(std::weak_ptr<BaseGameLogic> self)
{
	pimpl->m_Self = self;
	pimpl->m_ActorFactory = vCreateActorFactory();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EvtDataRequestDestroyActor::s_EventType, pimpl, [this](const IEventData & e){pimpl->onRequestDestroyActor(e); });

	cocos2d::Director::getInstance()->runWithScene(pimpl->m_Scene);
}

void BaseGameLogic::vUpdate(const std::chrono::milliseconds & deltaTimeMs)
{
	//Update the process runner.
	pimpl->m_ProcessRunner.updateAllProcess(deltaTimeMs);

	//Lock the lock of updating actors.
	pimpl->m_IsUpdatingActors = true;

	//Update all of the actors.
	for (auto & idActorPair : pimpl->m_Actors)
		idActorPair.second->update(deltaTimeMs);

	//Unlock the lock of updating actors.
	pimpl->m_IsUpdatingActors = false;

	//Execute the operations that are blocked by the updating process.
	while (!pimpl->m_CachedOperations.empty()){
		pimpl->m_CachedOperations.front()();
		pimpl->m_CachedOperations.pop_front();
	}
}

bool BaseGameLogic::isActorAlive(ActorID actorId) const
{
	return pimpl->m_Actors.find(actorId) != pimpl->m_Actors.end();
}

std::shared_ptr<Actor> BaseGameLogic::getActor(ActorID actorId) const
{
	//Try to find the actor corresponding to the id.
	auto actorIter = pimpl->m_Actors.find(actorId);

	//If fail to find the actor, return nullptr.
	if (actorIter == pimpl->m_Actors.end()){
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to find the actor and return it.
	return actorIter->second;
}

std::shared_ptr<Actor> BaseGameLogic::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
{
	//Try to create the actor.
	auto newActors = pimpl->m_ActorFactory->createActorAndChildren(resourceFile, overrides);

	//Failed to create the actor; return nullptr.
	if (newActors.empty()){
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to create the actor. Add it to actor map and return it.
	//std::map::emplace doesn't invalidate any iterators so it's safe to ignore the lock.
	auto parentActor = newActors[0];
	for (auto && newActor : newActors){
		auto emplaceResult = pimpl->m_Actors.emplace(std::make_pair(newActor->getID(), std::move(newActor)));
		assert(emplaceResult.second && "GameLogic::createActor() fail to emplace a new actor into the actor list.");
	}

	return parentActor;
}

void BaseGameLogic::addView(std::shared_ptr<BaseGameView> gameView)
{
	if (!gameView || gameView->isAttachedToLogic())
		return;

	assert(!pimpl->m_Self.expired() && "BaseGameLogic::addView() the logic is not initialized.");

	if (auto humanView = std::dynamic_pointer_cast<BaseHumanView>(gameView))
		pimpl->m_Scene->addChild(humanView->getSceneNode());

	gameView->setLogic(pimpl->m_Self);
	pimpl->m_GameViews.emplace(gameView->getID(), std::move(gameView));
}

std::shared_ptr<BaseHumanView> BaseGameLogic::getHumanView() const
{
	for (const auto & view : pimpl->m_GameViews)
		if (auto humanView = std::dynamic_pointer_cast<BaseHumanView>(view.second))
			return humanView;

	return nullptr;
}

std::shared_ptr<BaseGameView> BaseGameLogic::getGameView(GameViewID viewID) const
{
	const auto viewIter = pimpl->m_GameViews.find(viewID);
	if (viewIter == pimpl->m_GameViews.end())
		return nullptr;

	return viewIter->second;
}

void BaseGameLogic::removeView(GameViewID viewID)
{
	auto viewIter = pimpl->m_GameViews.find(viewID);
	if (viewIter == pimpl->m_GameViews.end())
		return;

	if (auto humanView = std::dynamic_pointer_cast<BaseHumanView>(viewIter->second))
		pimpl->m_Scene->removeChild(humanView->getSceneNode());

	viewIter->second->setLogic(std::weak_ptr<BaseGameLogic>());
	pimpl->m_GameViews.erase(viewIter);
}
