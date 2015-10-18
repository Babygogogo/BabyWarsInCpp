#include <map>
#include <list>
#include <cassert>
#include <mutex>
#include <functional>

#include "cocos2d.h"

#include "GameLogic.h"
#include "../Actor/Actor.h"
#include "../Actor/ActorFactory.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../Graphic2D/SceneStack.h"
#include "../Process/ProcessRunner.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameLogicImpl.
//////////////////////////////////////////////////////////////////////////
struct GameLogic::GameLogicImpl
{
public:
	GameLogicImpl();
	~GameLogicImpl();

	void onRequestDestroyActor(const IEventData & e);

	ProcessRunner m_ProcessRunner;

	//Lock of the process of updating actor.
	bool m_IsUpdatingActors{ false };

	std::map<ActorID, std::shared_ptr<Actor>> m_Actors;
	std::unique_ptr<ActorFactory> m_ActorFactory{ std::make_unique<ActorFactory>() };

	//The operations that are blocked because of the lock. They should be executed when the lock is unlocked.
	std::list<std::function<void()>> m_CachedOperations;
};

GameLogic::GameLogicImpl::GameLogicImpl()
{
}

GameLogic::GameLogicImpl::~GameLogicImpl()
{
}

void GameLogic::GameLogicImpl::onRequestDestroyActor(const IEventData & e)
{
	auto actorID = (static_cast<const EvtDataRequestDestoryActor &>(e)).getActorID();

	//Make sure that the actor to destroy is not the running scene.
	assert(actorID != SingletonContainer::getInstance()->get<SceneStack>()->getCurrentSceneID() && "GameLogicImpl::onRequestDestroyActor() trying to destroy the running scene!");

	if (m_IsUpdatingActors)
		m_CachedOperations.emplace_back([actorID, this](){m_Actors.erase(actorID); });
	else
		m_Actors.erase(actorID);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameLogic.
//////////////////////////////////////////////////////////////////////////
GameLogic::GameLogic() : pimpl{ std::make_shared<GameLogicImpl>() }
{
	static int InstanceCount{ 0 };
	assert((InstanceCount++ == 0) && "GameLogic is created more than once!");

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::RequestDestoryActor, pimpl, [this](const IEventData & e){pimpl->onRequestDestroyActor(e); });
}

GameLogic::~GameLogic()
{
}

void GameLogic::vUpdate(const std::chrono::milliseconds & deltaTimeMs)
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

bool GameLogic::isActorAlive(const ActorID & id) const
{
	return pimpl->m_Actors.find(id) != pimpl->m_Actors.end();
}

const std::shared_ptr<Actor> GameLogic::getActor(const ActorID & id) const
{
	//Try to find the actor corresponding to the id.
	auto actorIter = pimpl->m_Actors.find(id);

	//If fail to find the actor, return nullptr.
	if (actorIter == pimpl->m_Actors.end()){
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to find the actor and return it.
	return actorIter->second;
}

const std::shared_ptr<Actor> GameLogic::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
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
