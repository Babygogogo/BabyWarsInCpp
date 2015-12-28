#include <map>
#include <list>
#include <cassert>
#include <mutex>
#include <functional>

#include "../cocos2d/external/tinyxml2/tinyxml2.h"

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

	void destroyActors(const std::list<ActorID> actorIDs);

	ProcessRunner m_ProcessRunner;

	//Lock of the process of updating actor.
	bool m_IsUpdatingActors{ false };

	std::weak_ptr<BaseGameLogic> m_Self;

	std::map<ActorID, std::shared_ptr<Actor>> m_Actors;
	std::map<GameViewID, std::shared_ptr<BaseGameView>> m_GameViews;
	std::shared_ptr<BaseHumanView> m_HumanView;

	std::unique_ptr<BaseActorFactory> m_ActorFactory;

	//The operations that are blocked because of the lock. They should be executed when the lock is unlocked.
	std::list<std::function<void()>> m_CachedOperations;
};

BaseGameLogic::BaseGameLogicImpl::BaseGameLogicImpl()
{
}

BaseGameLogic::BaseGameLogicImpl::~BaseGameLogicImpl()
{
}

void BaseGameLogic::BaseGameLogicImpl::onRequestDestroyActor(const IEventData & e)
{
	const auto & requestDestroyActorEvent = static_cast<const EvtDataRequestDestroyActor &>(e);
	//Check if the actor to destroy is the running scene actor. Assert if yes.
	if (m_HumanView)
		if (auto sceneActor = m_HumanView->getSceneActor())
			assert(requestDestroyActorEvent.getActorID() != sceneActor->getID() && "BaseGameLogicImpl::onRequestDestroyActor() the actor to destory is the running scene actor.");

	//Generate the actor id list that should be removed.
	auto actorIDsToDestroy = std::list<ActorID>{ requestDestroyActorEvent.getActorID() };
	if (requestDestroyActorEvent.isAlsoDestroyChildren()) {
		for (const auto & id : actorIDsToDestroy) {
			auto actorIter = m_Actors.find(id);
			if (actorIter == m_Actors.end())
				continue;

			for (const auto & child : actorIter->second->getChildren())
				actorIDsToDestroy.emplace_back(child.first);
		}
	}

	if (m_IsUpdatingActors)
		m_CachedOperations.emplace_back([actorIDsToDestroy, this]() {destroyActors(actorIDsToDestroy); });
	else
		destroyActors(actorIDsToDestroy);
}

void BaseGameLogic::BaseGameLogicImpl::destroyActors(const std::list<ActorID> actorIDs)
{
	for (const auto & id : actorIDs)
		m_Actors.erase(id);
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

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EvtDataRequestDestroyActor::s_EventType, pimpl, [this](const IEventData & e) {pimpl->onRequestDestroyActor(e); });
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
	while (!pimpl->m_CachedOperations.empty()) {
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
	if (actorIter == pimpl->m_Actors.end()) {
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to find the actor and return it.
	return actorIter->second;
}

std::shared_ptr<Actor> BaseGameLogic::createActorAndChildren(const char * resourceFile, const tinyxml2::XMLElement * overrides /*= nullptr*/)
{
	//Load the resource file. Assert if failed.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(resourceFile);
	const auto rootElement = xmlDoc.RootElement();
	assert(rootElement && "BaseGameLogic::createActorAndChildren() can't load the xml file.");

	return createActorAndChildren(rootElement, overrides);
}

std::shared_ptr<Actor> BaseGameLogic::createActorAndChildren(const tinyxml2::XMLElement * actorElement, const tinyxml2::XMLElement * overrides /*= nullptr*/)
{
	auto newActors = pimpl->m_ActorFactory->createActorAndChildren(actorElement, overrides);
	if (newActors.empty()) {
		return nullptr;
	}

	//Succeed to create the actor. Add it to actor map and return it.
	//std::map::emplace doesn't invalidate any iterators so it's safe to ignore the lock.
	auto parentActor = newActors[0];
	for (auto && newActor : std::move(newActors)) {
		auto emplaceResult = pimpl->m_Actors.emplace(std::make_pair(newActor->getID(), std::move(newActor)));
		assert(emplaceResult.second && "GameLogic::createActorAndChildren() fail to emplace a new actor into the actor list.");
	}

	return parentActor;
}

void BaseGameLogic::addView(std::shared_ptr<BaseGameView> gameView)
{
	if (!gameView || gameView->isAttachedToLogic())
		return;

	assert(!pimpl->m_Self.expired() && "BaseGameLogic::addView() the logic is not initialized.");

	if (auto humanView = std::dynamic_pointer_cast<BaseHumanView>(gameView))
		assert("BaseGameLogic::addView() the view is a human view. Please call setHumanView() instead.");

	gameView->setLogic(pimpl->m_Self);
	pimpl->m_GameViews.emplace(gameView->getID(), std::move(gameView));
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

	viewIter->second->setLogic(std::weak_ptr<BaseGameLogic>());
	pimpl->m_GameViews.erase(viewIter);
}

void BaseGameLogic::setHumanView(const std::shared_ptr<BaseHumanView> & humanView)
{
	assert(humanView && "BaseGameLogic::setHumanView() the view is nullptr.");

	humanView->setLogic(pimpl->m_Self);
	pimpl->m_HumanView = humanView;
}

void BaseGameLogic::setHumanView(std::shared_ptr<BaseHumanView> && humanView)
{
	assert(humanView && "BaseGameLogic::setHumanView() the view is nullptr.");

	humanView->setLogic(pimpl->m_Self);
	pimpl->m_HumanView = std::move(humanView);
}

std::shared_ptr<BaseHumanView> BaseGameLogic::getHumanView() const
{
	return pimpl->m_HumanView;
}
